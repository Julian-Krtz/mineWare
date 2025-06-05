# mineWare

**mineWare** is the custom-built operating system for a proximity-triggered mine. It manages the mine’s behavior through a state machine, handling arming delays, trigger activation, and servo control, all using a single button and an LED for feedback.

---

## Getting Started

When the mine is powered on, it starts in a safe and unarmed state. In this state, the servo immediately moves into its default safe position. The LED blinks three times quickly to signal that the system has started and is currently unarmed.

To begin the arming process, press the button briefly. Each short press (less than one second) increases the arming delay by 3 seconds. For example, pressing the button three times will set a delay of 9 seconds before the mine becomes active. This allows you to leave the vicinity before the mine arms.

Once you stop pressing the button, the mine enters the arming phase. During this time, the LED blinks once per second, giving you a visual countdown. At the end of the delay, the LED lights up for one full second to signal that the mine is now fully armed and ready to trigger.

When the proximity sensor is activated—depending on your setup, this could be motion, distance, or another input—the mine immediately triggers. The servo moves to the trigger position, simulating a detonation or other custom action. At this point, the mine remains in the triggered state and requires manual reset to become safe again.

To deactivate the mine at any time, whether it's unarmed, armed, or triggered, hold the button for at least one full second. This brings the system back to its initial unarmed state and returns the servo to the safe position. From there, you can rearm the mine as described above.

---

## Summary of Behavior

- **Startup (Unarmed):** Servo moves to safe position; LED blinks 3× quickly.
- **Button Presses (< 1s):** Adds 3 seconds to the arming delay per press.
- **Arming Phase:** LED blinks once per second.
- **Armed State:** LED lights up for 1 second, then waits for trigger.
- **Triggered:** Servo moves to trigger position.
- **Long Button Press (≥ 1s):** Deactivates the mine from any state, and puts it in the unarmed state.
