package com.example.iot_backend.mqtt;


import org.springframework.stereotype.Component;

import java.util.*;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.ConcurrentLinkedDeque;

@Component
public class MessageStore {
  private static final int MAX_PER_TOPIC = 200;

  // topic -> deque of payloads
  private final Map<String, Deque<String>> store = new ConcurrentHashMap<>();

  public void append(String topic, String payload) {
    store.computeIfAbsent(topic, t -> new ConcurrentLinkedDeque<>()).addLast(payload);
    Deque<String> q = store.get(topic);
    while (q.size() > MAX_PER_TOPIC) {
      q.pollFirst();
    }
  }

  public List<String> recent(String topic, int limit) {
    Deque<String> q = store.getOrDefault(topic, new ConcurrentLinkedDeque<>());
    List<String> all = new ArrayList<>(q);
    int size = all.size();
    int from = Math.max(0, size - limit);
    return all.subList(from, size);
  }

  public Set<String> topics() {
    return store.keySet();
  }

  public void clear(String topic) {
    store.remove(topic);
  }
}