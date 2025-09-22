package com.example.iot_backend.controller;


import com.example.iot_backend.mqtt.MessageStore;
import org.springframework.http.MediaType;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.RequestParam;
import org.springframework.web.bind.annotation.RestController;
import reactor.core.publisher.Flux;

import java.time.Duration;

@RestController
public class StreamController {

  private final MessageStore store;

  public StreamController(MessageStore store) {
    this.store = store;
  }

  // Client sẽ nhận dữ liệu dạng text/event-stream mỗi 1s
  @GetMapping(value = "/api/stream", produces = MediaType.TEXT_EVENT_STREAM_VALUE)
  public Flux<String> stream(@RequestParam String topic) {
    return Flux.interval(Duration.ofSeconds(1))
        .map(tick -> String.join("\n", store.recent(topic, 1))); // gửi message mới nhất nếu có
  }
}