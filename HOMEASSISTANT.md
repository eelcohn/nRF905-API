# Integration in Home Assistant
Full home assistant integration isn't ready yet, but freakshock88 has found a way to do this:

![HaCard](https://github.com/LaTrappe/nRF905-API/blob/master/images/HA_Card_Example.png)<br>

#### In secrets.yaml:
```
ventilation_status: 'http://192.168.x.y/api/test/fan/querydevice.json'

ventilation_turn_on_low: 'http://192.168.x.y/api/v2/fan/setspeed.json?speed=low'
ventilation_turn_on_medium: 'http://192.168.x.y/api/v2/fan/setspeed.json?speed=medium'
ventilation_turn_on_high: 'http://192.168.x.y/api/v2/fan/setspeed.json?speed=high'
ventilation_turn_on_max: 'http://192.168.x.y/api/v2/fan/setspeed.json?speed=max'

ventilation_turn_on_low_30_mins: 'http://192.168.x.y/api/v2/fan/setspeed.json?speed=low&timer=30'
ventilation_turn_on_medium_30_mins: 'http://192.168.x.y/api/v2/fan/setspeed.json?speed=medium&timer=30'
ventilation_turn_on_high_30_mins: 'http://192.168.x.y/api/v2/fan/setspeed.json?speed=high&timer=30'
ventilation_turn_on_max_30_mins: 'http://192.168.x.y/api/v2/fan/setspeed.json?speed=max&timer=30'

ventilation_user: admin
ventilation_pass: nrf905
```

The above commands do not work (correctly) in combination with a C02 sensor. Instead of that use the parameters below:

```
ventilation_status: 'http://192.168.x.y/api/test/fan/querydevice.json'

ventilation_turn_on_max_10_mins: 'http://192.168.x.y/api/v1/send.json?txaddr=c9815c4e&payload=0100164ffa0302040a00000000000000&timeout=10'
ventilation_turn_on_max_30_mins: 'http://192.168.x.y/api/v1/send.json?txaddr=c9815c4e&payload=0100164ffa0302041e00000000000000&timeout=10"
ventilation_turn_on_max_60_mins: 'http://192.168.x.y/api/v1/send.json?txaddr=c9815c4e&payload=0100164ffa0302043c00000000000000&timeout=10'
ventilation_auto: 'http://192.168.x.y/api/v1/send.json?txaddr=c9815c4e&payload=0100164ffa0302010000000000000000&timeout=10'
ventilation_user: admin
ventilation_pass: nrf905
```

Depending on existince of a CO2 sensor add the rest commands of your choice to your configuration

#### In configuration.yaml::
```
rest_command:
  ventilationsystem_low:
    url: !secret ventilation_turn_on_low
    username: !secret ventilation_user
    password: !secret ventilation_pass
  ventilationsystem_medium:
    url: !secret ventilation_turn_on_medium
    username: !secret ventilation_user
    password: !secret ventilation_pass
  ventilationsystem_medium_30mins:
    url: !secret ventilation_turn_on_medium_30_mins
    username: !secret ventilation_user
    password: !secret ventilation_pass
```

Add a sensor to your sensor file where E1 is the device of your ventilation. You can find this via http://192.168.x.y/api/test/fan/querydevice.json 

#### In sensors.yaml::
```
- platform: rest
  resource: !secret ventilation_status
  username: !secret ventilation_user
  password: !secret ventilation_pass
  name: ventilation mode
  value_template: "{{ value_json['devices']['E1']['speed'] }}"
  json_attributes_path: "$.devices.E1"
  json_attributes:
    - "voltage"
    - "percentage"
    - "timer"
    
- platform: template
  sensors:
    ventilation_voltage:
        value_template: "{{ state_attr('sensor.ventilatie_stand', 'voltage') | float }}"
        friendly_name: 'ventilation voltage' 
        unit_of_measurement: 'v'
    ventilation_percentage:
        value_template: "{{ state_attr('sensor.ventilatie_stand', 'percentage') | int }}"
        friendly_name: 'ventilation percentage' 
        unit_of_measurement: '%'
    ventilation_timer:
        value_template: "{{ state_attr('sensor.ventilatie_stand', 'timer') }}" 
        friendly_name: 'ventilation timer'     
```
Create a script for each command. 

#### In script.yaml::
```
ventilatie_10_min:
  alias: 10 mins
  sequence:
  - service: rest_command.ventilation_10_mins
  mode: single
```

And finaly add the scripts and sensors to a lovelace card (lovelace.yaml). Requires 'custom:vertical-stack-in-card' and 'custom:button-card'

#### Style of the buttons::
```
button_card_templates:
  button_basic:
    state_color: true
    aspect_ratio: 1/1
    show_name: true
    styles:
      card:
       - background-color: rgba(255, 255, 255, 0)
     
      grid:
       - grid-template-areas: '"i icon" "n n"'
      img_cell:
        - background-color: '#0A324C' #rgba(0,0,0, 0.8)
        - width: 70%
        - border-radius: 10%
      name:
        - margin: 5%
        - font-size: 15px
        #- font-family: 'Poppins,sans-serif'
  button_icon:
    template: button_basic
    show_entity_picture: false
    show_icon: true
    size: 55%
```


#### And the card itself::
```
- type: custom:vertical-stack-in-card
  title: Ventilatie
  cards:
    - type: horizontal-stack
      cards:
        - entity: script.ventilatie_10_min
          icon: mdi:clock-time-two
          tap_action:
            action: call-service
            service: script.turn_on
            service_data:
              entity_id: script.ventilatie_10_min
          type: 'custom:button-card'
          template: button_icon
        - entity: script.ventilatie_30_min
          icon: mdi:clock-time-six
          tap_action:
            action: call-service
            service: script.turn_on
            service_data:
              entity_id: script.ventilatie_30_min
          type: 'custom:button-card'
          template: button_icon                 
        - entity: script.ventilatie_60_min
          icon: mdi:clock-time-twelve
          tap_action:
            action: call-service
            service: script.turn_on
            service_data:
              entity_id: script.ventilatie_60_min
          type: 'custom:button-card'
          template: button_icon
        - entity: script.ventilatie_auto
          icon: mdi:timer-off
          tap_action:
            action: call-service
            service: script.turn_on
            service_data:
              entity_id: script.ventilatie_auto
          type: 'custom:button-card'
          template: button_icon                  
    - type: entities
      style: |
          ha-card {
            margin-top: -0%;
            background-color: rgba(255, 255, 255, 0);
              }            
      entities:
      - sensor.ventilatie_stand
      - sensor.ventilatie_percentage
      - sensor.ventilatie_timer
      - sensor.ventilatie_voltage

```
