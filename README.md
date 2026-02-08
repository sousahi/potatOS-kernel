🥔 potatOS v2.9.2 - Master Window Edition

potatOS is a custom 32-bit x86 hobbyist operating system designed for simplicity, style, and efficiency. This version features the new Blue Window System, offering a unique retro-aesthetic interface with professional terminal capabilities.
🚀 Features

    Window Manager: Custom-built UI with blue backgrounds and dynamic text coloring.

    Safety Lock: Windows stay open until you press ESC, preventing data loss.

    Master Command Line (CML): A robust shell for system interaction.

    Hardware Info: Real-time CPU vendor detection via cpuid.

    Retro Apps: Includes classic ASCII art games and animations like snake.window and cube.window.

    Performance: Extremely lightweight, written in pure C and Assembly.

🎨 Visual Identity
Element	Color	VGA Hex
Terminal Background	White	0xF
Window Background	Blue	0x1
Primary Text	Yellow	0xE
Alert/Titles	Red	0x4
Retro Content	Black	0x0
💻 Available Commands

The system operates using .window applications. Here are some of the main tools:
🛠️ System Tools

    help: Displays the command list.

    info: System specs and licensing.

    cpu: Identifies the processor.

    mem: Shows allocated memory.

    cls: Clears the Master Terminal.

🎮 Entertainment

    snake.window: Classic snake movement.

    cube.window: 3D rotating wireframe cube.

    matrix: Digital rain simulation.

    cat.window, dog.window, bird.window: ASCII companions.

⚡ Power Commands

    reboot: Cold reboot of the machine.

    shutdown: Halts the CPU safely.

🛠️ Build & Run

To compile the kernel, use gcc for x86 targets:

    Compile:
    Bash

    gcc -m32 -ffreestanding -c kernel.c -o kernel.o

    Link: Use your preferred linker script to output a flat binary or ISO.

    Run:
    Bash

    qemu-system-i386 -kernel kernel.bin

📜 License

Developed by Paulo. Distributed under the MIT License.

    "Paulo is the Master of this OS."
