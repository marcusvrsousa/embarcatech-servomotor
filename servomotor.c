#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"

#define SERVO_PIN 22
#define PWM_FREQUENCY_HZ 50
#define PWM_PERIOD_US 20000
#define WRAP_VALUE 25000

#define SERVO_ANGLE_180_US 1920
#define SERVO_ANGLE_90_US 1190
#define SERVO_ANGLE_0_US 400

#define SMOOTH_STEP_US 5
#define SMOOTH_DELAY_MS 10

void set_servo_pulse_width(uint gpio, uint16_t pulse_width_us) {
    uint slice_num = pwm_gpio_to_slice_num(gpio);
    uint16_t level = (pulse_width_us * WRAP_VALUE) / PWM_PERIOD_US;
    pwm_set_chan_level(slice_num, pwm_gpio_to_channel(gpio), level);
}

void setup_servo_pwm() {
    gpio_set_function(SERVO_PIN, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(SERVO_PIN);
    pwm_set_wrap(slice_num, WRAP_VALUE);
    pwm_set_clkdiv(slice_num, 125.0f);
    pwm_set_enabled(slice_num, true);
}

void smooth_servo_movement(uint gpio, uint16_t start_pulse, uint16_t end_pulse, int step, int delay_ms) {
    if (start_pulse < end_pulse) {
        for (uint16_t pulse = start_pulse; pulse <= end_pulse; pulse += step) {
            set_servo_pulse_width(gpio, pulse);
            sleep_ms(delay_ms);
        }
    } else {
        for (uint16_t pulse = start_pulse; pulse >= end_pulse; pulse -= step) {
            set_servo_pulse_width(gpio, pulse);
            sleep_ms(delay_ms);
        }
    }
}

void test_pwm_config() {
    printf("\n[TESTE] Configuração do PWM\n");
    uint slice_num = pwm_gpio_to_slice_num(SERVO_PIN);
    uint clkdiv = pwm_get_clkdiv(slice_num);
    uint wrap = pwm_get_wrap(slice_num);
    printf("Frequência esperada: 50Hz\n");
    printf("Clock divisor atual: %f\n", (float)clkdiv);
    printf("Wrap value atual: %d\n", wrap);
    if (wrap == WRAP_VALUE && (int)clkdiv == 125) {
        printf("✅ PWM configurado corretamente!\n");
    } else {
        printf("❌ Erro na configuração do PWM!\n");
    }
}

void test_servo_positions() {
    printf("\n[TESTE] Movimentação do Servo\n");
    printf("Testando 0°...\n");
    set_servo_pulse_width(SERVO_PIN, SERVO_ANGLE_0_US);
    sleep_ms(3000);
    printf("Testando 90°...\n");
    set_servo_pulse_width(SERVO_PIN, SERVO_ANGLE_90_US);
    sleep_ms(3000);
    printf("Testando 180°...\n");
    set_servo_pulse_width(SERVO_PIN, SERVO_ANGLE_180_US);
    sleep_ms(3000);
    printf("✅ Teste concluído!\n");
}

int main() {

    test_pwm_config();
    test_servo_positions();
    
    setup_servo_pwm();

    set_servo_pulse_width(SERVO_PIN, SERVO_ANGLE_0_US);
    sleep_ms(5000);

    set_servo_pulse_width(SERVO_PIN, SERVO_ANGLE_90_US);
    sleep_ms(5000);

    set_servo_pulse_width(SERVO_PIN, SERVO_ANGLE_180_US);
    sleep_ms(5000);

    while (true) {
        smooth_servo_movement(SERVO_PIN, SERVO_ANGLE_180_US, SERVO_ANGLE_0_US, SMOOTH_STEP_US, SMOOTH_DELAY_MS);
        smooth_servo_movement(SERVO_PIN, SERVO_ANGLE_0_US, SERVO_ANGLE_180_US, SMOOTH_STEP_US, SMOOTH_DELAY_MS);
    }
}