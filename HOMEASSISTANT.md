# Integration in Home Assistant
Full home assistant integration isn't ready yet, but freakshock88 has found a way to do this:
#### In automation.yaml::
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
    url: !secret ventilation_turn_on_medium_for_30mins
    username: !secret ventilation_user
    password: !secret ventilation_pass
```
#### In secrets.yaml:
```
ventilation_turn_on_low: 'http://192.168.x.y/api/v1/zehnder/setpower.json?power=low'
ventilation_turn_on_medium: 'http://192.168.x.y/api/v1/zehnder/setpower.json?power=medium'
ventilation_turn_on_medium_for_30mins: 'http://192.168.x.y/api/v1/zehnder/settimer.json?power=medium&minutes=30'
ventilation_user: admin
ventilation_pass: nrf905
```
