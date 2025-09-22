package com.example.iot_backend.controller;

import com.example.iot_backend.service.CommandService;
import com.fasterxml.jackson.databind.ObjectMapper;
import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.*;

import java.util.Map;

@RestController
@RequestMapping("/api/devices")
public class CommandController {
  private final CommandService commandService;
  private final ObjectMapper objectMapper;

  public CommandController(CommandService commandService, ObjectMapper objectMapper) {
    this.commandService = commandService;
    this.objectMapper = objectMapper;
  }

  @PostMapping("/{id}/command")
  public ResponseEntity<String> sendCommand(@PathVariable Long id, @RequestBody Map<String, Object> body) {
    try {
      String action = (String) body.get("action");
      if (action == null || action.trim().isEmpty()) {
        return ResponseEntity.badRequest().body("Action is required");
      }
      
      Integer duration = null;
      if (body.containsKey("duration")) {
        Object durationObj = body.get("duration");
        if (durationObj instanceof Number) {
          duration = ((Number) durationObj).intValue();
        }
      }
      
      if (duration != null) {
        commandService.sendCommandWithDuration(id, action, duration);
        return ResponseEntity.ok("Command sent: " + action + " with duration: " + duration + "s");
      } else {
        commandService.sendCommand(id, action);
        return ResponseEntity.ok("Command sent: " + action);
      }
    } catch (Exception e) {
      return ResponseEntity.status(500).body("Failed to send command: " + e.getMessage());
    }
  }

  @PostMapping("/{id}/command/custom")
  public ResponseEntity<String> sendCustomCommand(@PathVariable Long id, @RequestBody Object payload) {
    try {
      String jsonPayload;
      if (payload instanceof String) {
        jsonPayload = (String) payload;
      } else {
        // Convert object to JSON string
        jsonPayload = objectMapper.writeValueAsString(payload);
      }
      
      commandService.sendCustomCommand(id, jsonPayload);
      return ResponseEntity.ok("Custom command sent successfully");
    } catch (Exception e) {
      return ResponseEntity.status(500).body("Failed to send custom command: " + e.getMessage());
    }
  }
}