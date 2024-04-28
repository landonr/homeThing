# homeThing Actions

## Menu Actions

### homething_menu.up Action
Go up in the menu, or up on a TV

```yaml
on_...
  then:
    - homething_menu.up: homeThingMenu
```

### homething_menu.down Action
Play/Pause media player, or go down on a TV

```yaml
on_...
  then:
    - homething_menu.down: homeThingMenu
```

### homething_menu.left Action
Previous track on media player, or go left on a TV

```yaml
on_...
  then:
    - homething_menu.left: homeThingMenu
```

### homething_menu.right Action
Next track on media player, or go right on a TV

```yaml
on_...
  then:
    - homething_menu.right: homeThingMenu
```

### homething_menu.select Action
Select the current menu item

```yaml
on_...
  then:
    - homething_menu.select: homeThingMenu
```

### homething_menu.scroll_clockwise Action
Move the menu cursor down, volume up

```yaml
on_...
  then:
    - homething_menu.scroll_clockwise: homeThingMenu
```

### homething_menu.scroll_counter_clockwise Action
Move the menu cursor up, volume down

```yaml
on_...
  then:
    - homething_menu.scroll_counter_clockwise: homeThingMenu
```

### homething_menu.back Action
Go back in the menu

```yaml
on_...
  then:
    - homething_menu.back: homeThingMenu
```

### homething_menu.option Action
Open the options menu

```yaml
on_...
  then:
    - homething_menu.option: homeThingMenu
```

### homething_menu.home Action
Go to the home screen

```yaml
on_...
  then:
    - homething_menu.home: homeThingMenu
```

## Notification Actions

### homething_menu.clear_notifications Action
Clears any existing notifications from homething_menu.

```yaml
on_...
  then:
    - homething_menu.clear_notifications: homeThingMenu
```

### homething_menu.add_notification Action
Adds a new notification
- **Title**: Text shown in notification header
- **Subtitle**: Text shown at the top of the content of notification
- **Message**: Text shown in content of notification
- **Persistent**: Notification stays on screen after screen lock (Optional: default false, set to true for a persistent notification)

```yaml
on_...
  then:
    - homething_menu.add_notification: 
        title: "Notification Title"
        subtitle: "Notification Subtitle"
        message: "Notification Message
        persistent: true
```
