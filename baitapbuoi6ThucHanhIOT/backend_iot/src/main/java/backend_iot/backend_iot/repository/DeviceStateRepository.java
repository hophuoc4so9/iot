package backend_iot.backend_iot.repository;

import backend_iot.backend_iot.model.DeviceState;
import org.springframework.data.jpa.repository.JpaRepository;
import org.springframework.stereotype.Repository;

import java.util.List;
import java.util.Optional;

@Repository
public interface DeviceStateRepository extends JpaRepository<DeviceState, Long> {
    List<DeviceState> findTop100ByOrderByTimestampDesc();
    Optional<DeviceState> findFirstByDeviceIdOrderByTimestampDesc(String deviceId);
    List<DeviceState> findByDeviceIdOrderByTimestampDesc(String deviceId);
}
