package com.example.iot_backend.service;

import com.example.iot_backend.model.Device;
import com.example.iot_backend.repository.DeviceRepository;
import org.springframework.data.domain.Sort;
import org.springframework.stereotype.Service;

import java.util.List;
import java.util.Optional;

@Service
public class DeviceService {
  private final DeviceRepository repo;

  public DeviceService(DeviceRepository repo) {
    this.repo = repo;
  }
// 3) Thêm sắp xếp theo createdAt giảm dần khi lấy danh sách.
  public List<Device> findAll() {
    return repo.findAll(Sort.by("createdAt").descending());
  }
// 1) Thêm API GET /api/devices/{id} trả về 1 thiết bị theo id
  public Optional<Device> findById(Long id) {
    return repo.findById(id);
  }
    public Device save(Device device) {
    return repo.save(device);
  }

  public void delete(Long id) {
    repo.deleteById(id);
  }
}