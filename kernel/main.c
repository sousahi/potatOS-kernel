volatile char *video = (char*)0xB8000;
int cursor = 0;
char command[80];
int cmd_idx = 0;

int sin_t[] = {0, 70, 100, 70, 0, -70, -100, -70};
int cos_t[] = {100, 70, 0, -70, -100, -70, 0, 70};
int cube_pts[8][3] = {
    {-4, -3,  4}, {4, -3,  4}, {4,  3,  4}, {-4,  3,  4},
    {-4, -3, -4}, {4, -3, -4}, {4,  3, -4}, {-4,  3, -4}
};

void outb(unsigned short port, unsigned char val) {
    asm volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

unsigned char inb(unsigned short port) {
    unsigned char ret;
    asm volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

int compare(char *a, char *b) {
    int i = 0;
    while(a[i] && b[i]) {
        if(a[i] != b[i]) return 0;
        i++;
    }
    return a[i] == b[i];
}

char get_ascii(unsigned char scan) {
    static char map[] = {
        0, 0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', 0, 0,
        'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', 0, 0,
        'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0, '\\',
        'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0, '*', 0, ' '
    };
    return (scan < 0x3B) ? map[scan] : 0;
}

void get_cpu_vendor(char *vendor) {
    unsigned int ebx, ecx, edx;
    asm volatile("cpuid" : "=b"(ebx), "=d"(edx), "=c"(ecx) : "a"(0));
    *((unsigned int *)vendor) = ebx;
    *((unsigned int *)(vendor + 4)) = edx;
    *((unsigned int *)(vendor + 8)) = ecx;
    vendor[12] = '\0';
}

void cls() {
    for(int i = 0; i < 80 * 25 * 2; i += 2) {
        video[i] = ' ';
        video[i + 1] = 0xF1; 
    }
    cursor = 0;
}

void print_at(int x, int y, const char *s, unsigned char color) {
    int pos = (y * 80 + x) * 2;
    for(int i = 0; s[i] != '\0'; i++) {
        if(s[i] == '\n') { y++; pos = (y * 80 + x) * 2; }
        else { video[pos++] = s[i]; video[pos++] = color; }
    }
}

void draw_window(const char* title) {
    cls();
    unsigned char win_bg = 0x1E; 
    for(int row = 4; row < 20; row++) {
        for(int col = 5; col < 72; col++) {
            int p = (row * 80 + col) * 2;
            video[p] = ' '; video[p+1] = 0x10;
        }
    }
    print_at(5, 3, "+==================================================================+", win_bg);
    print_at(7, 3, "[ ", win_bg); print_at(9, 3, title, 0x14); print_at(9 + 15, 3, " ]", win_bg);
    for(int i = 4; i < 20; i++) {
        print_at(5, i, "|", win_bg);
        print_at(71, i, "|", win_bg);
    }
    print_at(5, 20, "+==================================================================+", win_bg);
}

void wait_esc() {
    print_at(25, 19, "Press ESC to close window...", 0x10);
    while(1) {
        if (inb(0x64) & 0x01) {
            if (inb(0x60) == 0x01) break;
        }
    }
    cls();
}

void run_app(const char* title, const char* content, unsigned char text_color) {
    draw_window(title);
    print_at(10, 6, content, text_color);
    wait_esc();
}

void matrix_effect() {
    cls();
    for(int i = 0; i < 500; i++) {
        print_at((i%80), (i/20)%25, "10 ", 0xF2);
        if(i % 8 == 0) print_at((i%80), (i/20)%25, "01 ", 0xFA);
        for(volatile int d = 0; d < 2000000; d++);
        if(inb(0x64) & 0x01) if(inb(0x60) == 0x01) break;
    }
    cls();
}

void snake_game() {
    draw_window("SNAKE.window");
    int x = 40, y = 12;
    while(1) {
        int pos = (y * 80 + x) * 2;
        video[pos] = '@'; video[pos+1] = 0x1E;
        for(volatile int d = 0; d < 8000000; d++);
        video[pos] = ' '; video[pos+1] = 0x10;
        x++; if(x > 68) x = 10;
        if(inb(0x64) & 0x01) if(inb(0x60) == 0x01) break;
    }
    cls();
}

void cube_anim() {
    cls();
    int angle = 0;
    while(1) {
        for(int i = 0; i < 80 * 25 * 2; i+=2) { video[i] = ' '; video[i+1] = 0xF1; }
        int s = sin_t[angle], c = cos_t[angle];
        for(int i = 0; i < 8; i++) {
            int rx = (cube_pts[i][0] * c - cube_pts[i][2] * s) / 100;
            int vx = 40 + (rx * 2), vy = 12 + cube_pts[i][1];
            int pos = (vy * 80 + vx) * 2;
            if(pos >= 0 && pos < 80*25*2) { video[pos] = '#'; video[pos+1] = 0xF4; }
        }
        angle = (angle + 1) % 8;
        for(volatile int d = 0; d < 6000000; d++);
        if(inb(0x64) & 0x01) if(inb(0x60) == 0x01) break;
    }
    cls();
}

void execute_command() {
    if(compare(command, "help")) {
        run_app("HELP.window", "Commands: cat, dog, bird, ghost, coffee, snake, star, moon, hack,\nalert, clock, date, mem, cpu, net, disk, task, root, sudo,\npaulo, bios, boot, os, ver, ping, top, bash, clean, fire,\ncube, matrix, info, cls, reboot", 0x1E);
    }
    else if(compare(command, "info")) {
        run_app("INFO.window", "  _ __   ___ | |_ __ _| |_ ___  ___\n | '_ \\ / _ \\| __/ _` | __/ _ \\/ __|\n | |_) | (_) | || (_| | || (_) \\__ \\\n | .__/ \\___/ \\__\\__,_|\\__\\___/|___/\n |_|                                \n ------------------------------------\n OS:      potatOS v2.9.1\n CPU:     x86_32\n LICENSE: MIT License\n (c) 2026 Paulo. Master Edition.", 0x1E);
    }
    else if(compare(command, "cat")) run_app("CAT.window", "  |\\__/,|   (`\\ \n  |o o  |_   ) )\n -(((---(((--------", 0x10);
    else if(compare(command, "dog")) run_app("DOG.window", "      __      _ \n     o'')}____// \n      `_/      ) \n      (_(_/-(_/  ", 0x10);
    else if(compare(command, "bird")) run_app("BIRD.window", "   (o> \n    //\\ \n    V_/_ ", 0x14);
    else if(compare(command, "ghost")) run_app("GHOST.window", "  .---. \n / o o \\ \n |  ~  | \n  ^---^ ", 0x10);
    else if(compare(command, "coffee")) run_app("COFFEE.window", "  (  ) \n   ) ( \n  |----| \n  |____| ", 0x10);
    else if(compare(command, "snake")) snake_game();
    else if(compare(command, "star")) run_app("STAR.window", "     * \n    *** \n     * ", 0x1E);
    else if(compare(command, "moon")) run_app("MOON.window", "   _..._ \n  .     . \n  :     : \n  ' ._. ' ", 0x1E);
    else if(compare(command, "hack")) run_app("HACK.window", "Accessing Mainframe... [OK]\nBypassing Firewall... [OK]", 0x14);
    else if(compare(command, "alert")) run_app("ALERT.window", "SYSTEM ALERT: POTATO LEVEL CRITICAL!", 0x14);
    else if(compare(command, "clock")) run_app("CLOCK.window", "Clock: 16:30:00 (Simulated)", 0x1E);
    else if(compare(command, "date")) run_app("DATE.window", "Date: 2026-02-05", 0x1E);
    else if(compare(command, "mem")) run_app("MEMORY.window", "Memory: 10GB Allocated", 0x1E);
    else if(compare(command, "cpu")) { char cpu[13]; get_cpu_vendor(cpu); run_app("CPU.window", cpu, 0x1E); }
    else if(compare(command, "net")) run_app("NETWORK.window", "No network card found.", 0x14);
    else if(compare(command, "disk")) run_app("DISK.window", "Disk: IDE 0 [Connected]", 0x1E);
    else if(compare(command, "task")) run_app("TASKS.window", "PID 1: Kernel\nPID 2: CML Shell", 0x10);
    else if(compare(command, "root")) run_app("ROOT.window", "Permission Denied: You are already God.", 0x14);
    else if(compare(command, "sudo")) run_app("SUDO.window", "sudo: command not found (Try 'paulo')", 0x14);
    else if(compare(command, "paulo")) run_app("MASTER.window", "Paulo is the Master of this OS.", 0x1E);
    else if(compare(command, "ver")) run_app("VERSION.window", "Version 2.9.1 Build 2026", 0x1E);
    else if(compare(command, "ping")) run_app("PING.window", "PING 127.0.0.1: time=0.01ms", 0x1E);
    else if(compare(command, "fire")) run_app("FIRE.window", " ( ) \n (   ) \n(     )", 0x14);
    else if(compare(command, "cube")) cube_anim();
    else if(compare(command, "matrix")) matrix_effect();
    else if(compare(command, "cls")) { cls(); }
    else if(compare(command, "reboot")) outb(0x64, 0xFE);
    else if(compare(command, "shutdown")) { cls(); print_at(0, 0, "Off.", 0xF4); while(1) asm("hlt"); }
    else { cls(); print_at(0, 0, "Error: Command not found.", 0xF4); }

    print_at(0, 24, "CML:$ ", 0xF1);
    for(int i=0; i<80; i++) command[i] = 0;
    cmd_idx = 0;
}

void kernel_main() {
    cls();
    print_at(0, 0, "potatOS v2.9.1 Master Edition", 0xF1); 
    print_at(0, 24, "CML:$ ", 0xF1);
    while(1) {
        if (inb(0x64) & 0x01) {
            unsigned char scan = inb(0x60);
            if (!(scan & 0x80)) {
                if (scan == 0x1C) {
                    command[cmd_idx] = '\0'; execute_command(); 
                }
                else if (scan == 0x0E && cmd_idx > 0) {
                    cmd_idx--; 
                    video[(24 * 80 + (6 + cmd_idx)) * 2] = ' ';
                }
                else { 
                    char c = get_ascii(scan); 
                    if (c && cmd_idx < 70) { 
                        command[cmd_idx++] = c;
                        print_at(6 + cmd_idx - 1, 24, &c, 0xF1);
                    } 
                }
            }
        }
    }
}
