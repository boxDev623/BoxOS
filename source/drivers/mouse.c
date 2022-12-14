#include <drivers/mouse.h>
#include <drivers/vesa.h>

#include <interrupt/8259_pic.h>
#include <interrupt/idt.h>
#include <interrupt/isr.h>

#include <kcsl.h>
#include <string.h>
#include <io_ports.h>

float g_mouse_x_pos = 0, g_mouse_y_pos = 0;
float mouse_speed = 6;
uint32_t under_mouse_buffer = NULL;

MouseStatus g_status;

int Mouse_GetX(void)
{
    return g_mouse_x_pos;
}

int Mouse_GetY(void)
{
    return g_mouse_y_pos;
}

MouseStatus Mouse_GetStatus(void)
{
    return g_status;
}

void mouse_wait(bool type)
{
    /*uint32_t time_out = 100000;
    if (type == false)
    {
        // suspend until status is 1
        while (time_out--)
            if ((inportb(PS2_CMD_PORT) & 1) == 1)
                return;
        return;
    }
    else
    {
        while (time_out--)
            if ((inportb(PS2_CMD_PORT) & 2) == 0)
                return;
    }*/

    return;
}

void mouse_write(uint8_t data)
{
    mouse_wait(true);
    outportb(PS2_CMD_PORT, 0xD4);
    mouse_wait(true);
    outportb(MOUSE_DATA_PORT, data);
}

uint8_t mouse_read(void)
{
    mouse_wait(false);
    return inportb(MOUSE_DATA_PORT);
}

void get_mouse_status(char status_byte, MouseStatus *status) 
{
    memset(status, 0, sizeof(MouseStatus));
    if (status_byte & 0x01)
        status->left_button = 1;
    if (status_byte & 0x02)
        status->right_button = 1;
    if (status_byte & 0x04)
        status->middle_button = 1;
    if (status_byte & 0x08)
        status->always_1 = 1;
    if (status_byte & 0x10)
        status->x_sign = 1;
    if (status_byte & 0x20)
        status->y_sign = 1;
    if (status_byte & 0x40)
        status->x_overflow = 1;
    if (status_byte & 0x80)
        status->y_overflow = 1;
}

void print_mouse_info(void)
{
    kprintf("Mouse X: %d, Y: %d\n", g_mouse_x_pos, g_mouse_y_pos);
    if (g_status.left_button)
        kprintf("Left button clicked");
    if (g_status.right_button)
        kprintf("Right button clicked");
    if (g_status.middle_button)
        kprintf("Middle button clicked");
}

void mouse_handler(Registers *regs)
{
    static uint8_t mouse_cycle = 0;
    float prev_mouse_x_pos = g_mouse_x_pos, prev_mouse_y_pos = g_mouse_y_pos;

    static char mouse_byte[3];
    switch (mouse_cycle)
    {
        case 0:
            mouse_byte[0] = mouse_read();
            get_mouse_status(mouse_byte[0], &g_status);
            mouse_cycle++;
            break;
        case 1:
            mouse_byte[1] = mouse_read();
            mouse_cycle++;
            break;
        case 2:
            mouse_byte[2] = mouse_read();
            g_mouse_x_pos = g_mouse_x_pos + (int)(mouse_byte[1] * (mouse_speed / 10.0f));
            g_mouse_y_pos = g_mouse_y_pos - (int)(mouse_byte[2] * (mouse_speed / 10.0f));

            if (g_mouse_x_pos < 0)
                g_mouse_x_pos = 0;
            if (g_mouse_y_pos < 0)
                g_mouse_y_pos = 0;

            if (g_mouse_x_pos > (int)VBE_GetWidth())
                g_mouse_x_pos = (int)VBE_GetWidth() - 1;
            if (g_mouse_y_pos > (int)VBE_GetHeight())
                g_mouse_y_pos = (int)VBE_GetHeight() - 1;
            
            mouse_cycle = 0;
            break;
    }
    ISR_EndInterrupt(IRQ_BASE + 12);
}

// Available rates are 10, 20, 40, 60, 80, 100, 200
void set_mouse_rate(uint8_t rate)
{
    uint8_t status;

    outportb(MOUSE_DATA_PORT, MOUSE_CMD_SAMPLE_RATE);
    status = mouse_read();
    if(status != MOUSE_ACKNOWLEDGE)
    {
        kprintf("error: failed to send mouse sample rate command\n");
        return;
    }
    outportb(MOUSE_DATA_PORT, rate);
    status = mouse_read();
    if(status != MOUSE_ACKNOWLEDGE)
    {
        kprintf("error: failed to send mouse sample rate data\n");
        return;
    }
}

void Mouse_Initialize(void)
{
    uint8_t status;

    g_mouse_x_pos = 5;
    g_mouse_y_pos = 2;

    mouse_wait(true);
    outportb(PS2_CMD_PORT, 0xA8);

    outportb(MOUSE_DATA_PORT, MOUSE_CMD_MOUSE_ID);
    status = mouse_read();

    set_mouse_rate(10);

    mouse_wait(true);
    outportb(PS2_CMD_PORT, 0x20);
    mouse_wait(false);

    status = (inportb(MOUSE_DATA_PORT) | 2);

    mouse_wait(true);
    outportb(PS2_CMD_PORT, MOUSE_DATA_PORT);
    mouse_wait(true);
    outportb(MOUSE_DATA_PORT, status);

    mouse_write(MOUSE_CMD_SET_DEFAULTS);
    status = mouse_read();
    if(status != MOUSE_ACKNOWLEDGE)
    {
        //kprintf("Error: Failed to set default mouse settings!\n");
        return;
    }

    mouse_write(MOUSE_CMD_ENABLE_PACKET_STREAMING);
    status = mouse_read();
    if(status != MOUSE_ACKNOWLEDGE)
    {
        //kprintf("Error: Failed to enable mouse packet streaming!\n");
        return;
    }

    ISR_RegisterInterruptHandler(IRQ_BASE + IRQ12_AUXILIARY, mouse_handler);
}