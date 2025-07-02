#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/device.h>
#include <zephyr/drivers/i2c.h>

#define MAX20356_I2C_ADDR 0x28
#define I2C_NODE DT_NODELABEL(i2c2)
#define SLEEP_TIME_MS 1000

//#define I2C_NODE DT_NODELABEL(i2c2)


//const struct device *i2c_dev = DEVICE_DT_GET(I2C_NODE);
//static const struct device *i2c_dev = DEVICE_DT_GET_ANY(I2C_NODE);

static const struct device *i2c_dev = DEVICE_DT_GET(I2C_NODE);


// --- I2C Register Write Helper ---
int max20356_write_reg(uint8_t reg, uint8_t val) {

    uint8_t buf[2] = { reg, val };
    return i2c_write(i2c_dev, buf, sizeof(buf), MAX20356_I2C_ADDR);
}

// --- I2C Register Read Helper (optional) ---
int max20356_read_reg(uint8_t reg, uint8_t *val) {
    return i2c_write_read(i2c_dev, MAX20356_I2C_ADDR, &reg, 1, val, 1);
}

// --- Enable BUCK1 (1V8_D), BUCK2 + LSW1 (1V8_S) for MAX20356A ---
void enable_1v8_rails(void) {
    int ret;

    // --- Optional: Unlock registers (if needed) ---
    max20356_write_reg(0x8A, 0x55);
    max20356_write_reg(0x8B, 0x55);
    max20356_write_reg(0x8C, 0x55);

    // --- BUCK1 (1V8_D) ---
    ret = max20356_write_reg(0x34, 0x3F);  // Set BUCK1 voltage to 1.8V
    if (ret < 0) { printk("Failed to set BUCK1 voltage: %d\n", ret); return; }

    ret = max20356_write_reg(0x30, 0xE1);  // Enable BUCK1
    if (ret < 0) { printk("Failed to enable BUCK1: %d\n", ret); return; }

    // --- BUCK2 (1V8_S) ---
    ret = max20356_write_reg(0x40, 0x3F);  // Set BUCK2 voltage to 1.8V
    if (ret < 0) { printk("Failed to set BUCK2 voltage: %d\n", ret); return; }

    ret = max20356_write_reg(0x3C, 0xE1);  // Enable BUCK2
    if (ret < 0) { printk("Failed to enable BUCK2: %d\n", ret); return; }

    // --- LSW1 (part of 1V8_S) ---
    //ret = max20356_write_reg(0x6A, 0x01);  // Configure LSW1 (basic control)
   // if (ret < 0) { printk("Failed to configure LSW1: %d\n", ret); return; }

    ret = max20356_write_reg(0x69, 0xE1);  // Enable LSW1
    if (ret < 0) { printk("Failed to enable LSW1: %d\n", ret); return; }

    // --- BBOUT  ---
   //  ret = max20356_write_reg(0x56, 0x32);  // 
   //  if (ret < 0) { printk("Failed to configure LSW1: %d\n", ret); return; }

    ret = max20356_write_reg(0x54, 0xE1);  // Enable BBOUT
    if (ret < 0) { printk("Failed to enable LSW1: %d\n", ret); return; }

    printk("Successfully enabled 1V8_D (BUCK1) and 1V8_S (BUCK2 + LSW1)\n");
}

int main(void) {
    printk("MAX20356 Sensor Rail Enable\n");

    if (!device_is_ready(i2c_dev)) {
        printk("I2C device not ready!\n");
        return 0;
    }

    // Power the required 1.8V rails
    enable_1v8_rails();

    while (1) {
        k_msleep(SLEEP_TIME_MS);
    }
}


