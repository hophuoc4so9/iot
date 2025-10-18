package backend_iot.backend_iot.repository;

import backend_iot.backend_iot.model.SensorData;
import org.springframework.data.jpa.repository.JpaRepository;
import org.springframework.stereotype.Repository;

import java.util.List;

@Repository
public interface SensorDataRepository extends JpaRepository<SensorData, Long> {
    List<SensorData> findTop100ByOrderByReceivedAtDesc();
}
