package com.example.iot_backend.mqtt;

import org.springframework.data.redis.core.RedisTemplate;
import org.springframework.stereotype.Component;

import java.util.*;
import java.util.concurrent.TimeUnit;

@Component
public class RedisMessageStore {
  private static final int MAX_PER_TOPIC = 200;
  private static final long EXPIRE_TIME_HOURS = 24; // Messages expire after 24 hours
  
  private final RedisTemplate<String, Object> redisTemplate;
  private final String TOPICS_SET_KEY = "mqtt:topics";
  private final String TOPIC_PREFIX = "mqtt:topic:";

  public RedisMessageStore(RedisTemplate<String, Object> redisTemplate) {
    this.redisTemplate = redisTemplate;
  }

  public void append(String topic, String payload) {
    String topicKey = TOPIC_PREFIX + topic;
    
    // Add to topics set
    redisTemplate.opsForSet().add(TOPICS_SET_KEY, topic);
    
    // Add message to topic list (left push for LIFO order)
    redisTemplate.opsForList().leftPush(topicKey, payload);
    
    // Trim list to keep only MAX_PER_TOPIC messages
    redisTemplate.opsForList().trim(topicKey, 0, MAX_PER_TOPIC - 1);
    
    // Set expiration for the topic key
    redisTemplate.expire(topicKey, EXPIRE_TIME_HOURS, TimeUnit.HOURS);
    
    // Also set expiration for topics set
    redisTemplate.expire(TOPICS_SET_KEY, EXPIRE_TIME_HOURS, TimeUnit.HOURS);
  }

  public List<String> recent(String topic, int limit) {
    String topicKey = TOPIC_PREFIX + topic;
    
    // Get recent messages (already in reverse chronological order due to leftPush)
    List<Object> messages = redisTemplate.opsForList().range(topicKey, 0, limit - 1);
    if (messages == null) {
      return new ArrayList<>();
    }
    
    // Convert to string list and reverse to get chronological order (oldest to newest)
    List<String> result = new ArrayList<>();
    for (int i = messages.size() - 1; i >= 0; i--) {
      result.add(messages.get(i).toString());
    }
    
    return result;
  }

  public Set<String> topics() {
    Set<Object> topicsSet = redisTemplate.opsForSet().members(TOPICS_SET_KEY);
    if (topicsSet == null) {
      return new HashSet<>();
    }
    
    Set<String> result = new HashSet<>();
    for (Object topic : topicsSet) {
      result.add(topic.toString());
    }
    return result;
  }

  public void clear(String topic) {
    String topicKey = TOPIC_PREFIX + topic;
    redisTemplate.delete(topicKey);
    redisTemplate.opsForSet().remove(TOPICS_SET_KEY, topic);
  }

  public void clearAll() {
    // Get all topics
    Set<String> allTopics = topics();
    
    // Delete all topic keys
    for (String topic : allTopics) {
      String topicKey = TOPIC_PREFIX + topic;
      redisTemplate.delete(topicKey);
    }
    
    // Delete topics set
    redisTemplate.delete(TOPICS_SET_KEY);
  }
}