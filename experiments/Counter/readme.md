# Counter Project - TouchGFX

This project is a simple counter application implemented using **TouchGFX Designer** and **STM32CubeIDE**.

---

## Project Overview

- Displays a counter on a TouchGFX UI.
- Allows you to increment or decrement the counter with a button press.
- Shows how to use **wildcards** in TextArea widgets to reflect dynamic values.

---

## Features

- **Dynamic Text Display** with wildcards.
- **User Interaction** with increment and decrement buttons.
- **Clean UI-Logic Separation** — UI designed in TouchGFX Designer; application implemented in C++.

---

## How It Works

- The TextArea widget contains a wildcard (like `%s`) in TouchGFX Designer.
- The application maintains a buffer for this wildcard and refreshes it when the counter is updated.

---

## Simulation Video

Watch the counter in action on YouTube:  
[**View video on YouTube**](https://youtu.be/llw6tRPuS-E)

---

## Code Sketch (Key Parts)

```cpp
void Screen1View::down_clicked()
{
counter--;
if (counter<=0) counter = 0;
Unicode::snprintf(textArea1Buffer, TEXTAREA1_SIZE, "%d", counter);
textArea1.invalidate();
}
void Screen1View::up_clicked()
{
counter++;
Unicode::snprintf(textArea1Buffer, TEXTAREA1_SIZE, "%d", counter);
textArea1.invalidate();
}
```

## How to Build and Run

1. Open in **STM32CubeIDE**.
2. Generate code with **TouchGFX Designer**.
3. Build and flash to your STM32 board or run the TouchGFX Simulator.
4. Use the UI to increment or decrement the counter.

---

✨ Feel free to reuse or modify this example for your own projects! 
