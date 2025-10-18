package btbuoi6.btbuoi6.service;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.integration.support.MessageBuilder;
import org.springframework.messaging.MessageChannel;
import org.springframework.stereotype.Service;

@Service
public class MqttPublisherService {
    
    @Autowired
    private MessageChannel mqttOutboundChannel;

    public void publish(String topic, String payload) {
        mqttOutboundChannel.send(
            MessageBuilder.withPayload(payload)
                .setHeader("mqtt_topic", topic)
                .build()
        );
    }
}
