# 🧪 Lab 7 (Chi tiết): Tối ưu lưu trữ dữ liệu Telemetry (PostgreSQL)

> **Mục tiêu cuối buổi:**  
> - Biết cách **tạo index** cho bảng `telemetry`.  
> - Biết cách **chia bảng theo thời gian (partition)**.  
> - **Đo và so sánh** tốc độ truy vấn **trước** và **sau** khi tối ưu.  
>
> **Yêu cầu:** Hoàn thành Lab 4 (đã có bảng `telemetry`, có dữ liệu JSON `data`, trường thời gian `ts`, và `device_id`).

---

## 🧭 0) Kiến trúc & Tư duy tối ưu
Dữ liệu IoT có đặc trưng: **dòng thời gian**, **ghi nhiều/đọc theo khoảng thời gian**, và **truy vấn gần đây**.  
Các kỹ thuật chính trong Lab:
1. **Index phù hợp** cho các cột lọc/sắp xếp thường dùng (`device_id`, `ts`).  
2. **Partition theo thời gian** để giảm kích thước mỗi partition → scan nhanh hơn.  
3. (Tùy chọn) **GIN trên JSONB** nếu truy vấn sâu vào payload.  
4. (Nâng cao) Sử dụng **TimescaleDB** (hypertable) nếu dữ liệu rất lớn.

---

## ✅ 1) Chuẩn bị dữ liệu mẫu đủ lớn
Nếu DB ít dữ liệu, hãy **tạo dữ liệu mô phỏng** để thấy rõ hiệu quả.  
Chạy các lệnh SQL sau (PostgreSQL):

> ⚠️ Thực hiện trên **môi trường phát triển**, tránh chạy trên môi trường sản xuất.

```sql
-- 1) Tạo nhiều device giả lập (nếu chưa có)
INSERT INTO devices (name, type, status, created_at)
SELECT 'Device ' || gs::text, 'sensor', 'ONLINE', now()
FROM generate_series(1, 200) AS gs;

-- 2) Sinh ~1 triệu bản ghi telemetry (tùy máy, có thể giảm)
--   - 200 thiết bị, mỗi thiết bị 5k mẫu => 1,000,000 rows
--   - ts phân bố trong 60 ngày gần đây
WITH dev AS (
  SELECT id FROM devices ORDER BY id LIMIT 200
),
g AS (
  SELECT d.id AS device_id, gs AS seq
  FROM dev d
  CROSS JOIN generate_series(1, 5000) gs
)
INSERT INTO telemetry (device_id, ts, data)
SELECT 
  g.device_id,
  now() - (interval '60 days') * random(),
  json_build_object(
    'temp', round(20 + random()*10, 2),
    'hum', round(40 + random()*20, 2)
  )::text
FROM g;
```

> **Ghi chú:** Nếu dùng kiểu cột `data JSONB` thay vì `text`, đổi `::text` thành `::jsonb`.

Kiểm tra số lượng:
```sql
SELECT count(*) FROM telemetry;
```

---

## 🧱 2) Tạo các Index quan trọng
Mẫu truy vấn phổ biến:
- Lấy **telemetry gần nhất** theo `device_id`.
- Lấy dữ liệu **theo khoảng thời gian**.
- Lọc theo **thuộc tính** trong JSON (`data->>'temp'`).

### 2.1 Index theo `(device_id, ts DESC)`
Phù hợp để truy vấn mới nhất hoặc ORDER BY ts desc.

```sql
-- Btree có thứ tự; DESC để tối ưu ORDER BY ts DESC
CREATE INDEX IF NOT EXISTS idx_telemetry_device_ts_desc
ON telemetry (device_id, ts DESC);
```

### 2.2 Index chỉ theo `ts` (tuỳ nhu cầu)
Hữu ích cho truy vấn theo **khoảng thời gian** khi không lọc device.

```sql
CREATE INDEX IF NOT EXISTS idx_telemetry_ts
ON telemetry (ts);
```

### 2.3 (Tuỳ chọn) GIN index trên JSONB
Nếu cột `data` là **JSONB** và bạn truy vấn theo keys bên trong, thêm GIN:

```sql
-- Chỉ dùng nếu data là JSONB
CREATE INDEX IF NOT EXISTS idx_telemetry_data_gin
ON telemetry USING GIN ((data));
```

Hoặc index expression cho trường cụ thể (ví dụ temp):
```sql
-- Tối ưu cho điều kiện WHERE (data->>'temp')::numeric > 30
CREATE INDEX IF NOT EXISTS idx_telemetry_temp_num
ON telemetry (((data->>'temp')::numeric));
```

> **Chọn lọc:** Không tạo quá nhiều index, vì mỗi index làm **tăng chi phí ghi**.

---

## 🧩 3) Đo hiệu năng trước & sau (EXPLAIN ANALYZE)
### 3.1 Câu truy vấn 1 — Lấy mẫu mới nhất cho 1 device
```sql
EXPLAIN ANALYZE
SELECT id, device_id, ts, data
FROM telemetry
WHERE device_id = 42
ORDER BY ts DESC
LIMIT 50;
```

Kỳ vọng: Sau khi có index `(device_id, ts DESC)`, PostgreSQL dùng **Index Scan**, độ trễ giảm đáng kể.

### 3.2 Câu truy vấn 2 — Lấy dữ liệu theo khoảng thời gian
```sql
EXPLAIN ANALYZE
SELECT id, device_id, ts, data
FROM telemetry
WHERE ts BETWEEN now() - interval '7 days' AND now();
```

Kỳ vọng: Dùng **Bitmap Index Scan** trên `idx_telemetry_ts` nếu đủ lợi ích.

### 3.3 Câu truy vấn 3 — Lọc theo nhiệt độ (nâng cao)
Nếu `data` là JSONB:
```sql
EXPLAIN ANALYZE
SELECT id, device_id, ts, data
FROM telemetry
WHERE (data->>'temp')::numeric > 28
AND ts > now() - interval '7 days';
```

Kỳ vọng: Index expression `idx_telemetry_temp_num` + `idx_telemetry_ts` giúp giảm thời gian.

> **Ghi chú:** Dùng `EXPLAIN ANALYZE` để so sánh **thời gian thực thi** và **kế hoạch thực thi**.

---

## 🧱 4) Partition theo thời gian (RANGE partition by month)

### 4.1 Tạo bảng mẹ (parent table)
> ⚠️ Sao lưu dữ liệu trước khi đổi schema. Nếu đã có bảng `telemetry`, bạn có thể tạo bảng mới `telemetry2` để demo.

```sql
-- Nếu cần, tạo bảng mới để demo partition
DROP TABLE IF EXISTS telemetry2 CASCADE;

CREATE TABLE telemetry2 (
  id BIGSERIAL PRIMARY KEY,
  device_id BIGINT NOT NULL REFERENCES devices(id),
  ts TIMESTAMP NOT NULL,
  data JSONB
) PARTITION BY RANGE (ts);
```

### 4.2 Tạo partition theo tháng
Ví dụ tạo partition cho 2025-06 đến 2025-10:

```sql
CREATE TABLE telemetry2_2025_06 PARTITION OF telemetry2
  FOR VALUES FROM ('2025-06-01') TO ('2025-07-01');
CREATE TABLE telemetry2_2025_07 PARTITION OF telemetry2
  FOR VALUES FROM ('2025-07-01') TO ('2025-08-01');
CREATE TABLE telemetry2_2025_08 PARTITION OF telemetry2
  FOR VALUES FROM ('2025-08-01') TO ('2025-09-01');
CREATE TABLE telemetry2_2025_09 PARTITION OF telemetry2
  FOR VALUES FROM ('2025-09-01') TO ('2025-10-01');
CREATE TABLE telemetry2_2025_10 PARTITION OF telemetry2
  FOR VALUES FROM ('2025-10-01') TO ('2025-11-01');
```

> **Mẹo:** Dùng script sinh partition tự động mỗi tháng.

### 4.3 Tạo index trên **mỗi partition**
Index **không tự áp vào** partition con — cần tạo **riêng**:

```sql
-- Index cho từng partition (tối thiểu theo device_id, ts)
CREATE INDEX idx_t2_2025_09_device_ts_desc ON telemetry2_2025_09 (device_id, ts DESC);
-- ...lặp lại cho các partition khác
```

### 4.4 Sao chép dữ liệu (demo)
```sql
INSERT INTO telemetry2 (device_id, ts, data)
SELECT device_id, ts, data::jsonb
FROM telemetry
WHERE ts >= '2025-06-01' AND ts < '2025-11-01';
```

### 4.5 Kiểm tra pruning (cắt bớt partition)
```sql
EXPLAIN ANALYZE
SELECT count(*)
FROM telemetry2
WHERE ts BETWEEN '2025-09-01' AND '2025-09-15';
```
Kỳ vọng: Kế hoạch chỉ **scan partition tháng 09**, thay vì quét toàn bộ.

---

## 🚀 5) So sánh tốc độ (gợi ý kịch bản)
1. Chạy truy vấn **trước khi** tạo index/partition, ghi lại thời gian.  
2. Tạo index thích hợp → chạy lại, ghi thời gian.  
3. Di chuyển dữ liệu sang bảng partition `telemetry2` (hoặc tạo partition trực tiếp) → chạy lại, so sánh.  
4. Đánh giá: **Index** giúp **lọc nhanh theo device/time**; **Partition** giúp **giảm không gian scan** khi truy vấn theo thời gian.

> **Lưu ý:** Hiệu năng phụ thuộc số bản ghi, cấu hình máy, bộ nhớ, cache, v.v.

---

## 🧪 6) (Nâng cao) TimescaleDB (tuỳ chọn)
Nếu sử dụng **TimescaleDB**, thao tác đơn giản hơn:

```sql
-- Cài extension
CREATE EXTENSION IF NOT EXISTS timescaledb;

-- Chuyển bảng telemetry thành hypertable
SELECT create_hypertable('telemetry', 'ts', chunk_time_interval => INTERVAL '7 days');

-- Index theo (device_id, ts DESC)
CREATE INDEX IF NOT EXISTS idx_ts_device_ts_desc ON telemetry (device_id, ts DESC);
```

**Ưu điểm:** TimescaleDB tự quản lý chunk (tương tự partition), hỗ trợ **compression**, **continuous aggregates**, v.v.

---

## 🛠️ 7) Lỗi thường gặp & khắc phục
| Hiện tượng | Nguyên nhân | Cách xử lý |
|---|---|---|
| Insert chậm | Quá nhiều index | Xem lại index, giữ những index phục vụ truy vấn chính |
| Query không dùng index | Hàm/ép kiểu trong WHERE | Dùng **index expression** phù hợp, tránh hàm bên trái điều kiện |
| Partition vẫn scan nhiều | Điều kiện WHERE chưa đủ cụ thể | Sử dụng điều kiện chặt chẽ theo `ts` để kích hoạt pruning |
| JSONB query chậm | Không có GIN / expression index | Tạo index phù hợp cho keys thường truy vấn |
| Tạo partition thủ công vất vả | Thiếu tự động hoá | Viết script sinh partition theo tháng / dùng TimescaleDB |

---

## 🏁 Kết quả cuối cùng
- Đã tạo index phù hợp cho truy vấn thiết bị & thời gian.  
- Đã cấu hình partition theo tháng và thấy **pruning** hoạt động.  
- Quan sát được **tốc độ truy vấn cải thiện rõ rệt** với `EXPLAIN ANALYZE`.

---

## 📚 Bài tập củng cố
1) Tạo **view** `latest_telemetry` trả về bản ghi mới nhất cho mỗi `device_id`.  
2) Benchmark 3 truy vấn thực tế của bạn (theo device, theo khoảng thời gian, theo temp>ngưỡng) trước/sau tối ưu và nộp kết quả.  
3) (Nâng cao) Bật **compression** của TimescaleDB với dữ liệu cũ (>30 ngày) và so sánh dung lượng.
