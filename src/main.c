#include <zephyr/kernel.h>             // Funções básicas do Zephyr (ex: k_msleep, k_thread, etc.)
#include <zephyr/device.h>             // API para obter e utilizar dispositivos do sistema
#include <zephyr/drivers/gpio.h>       // API para controle de pinos de entrada/saída (GPIO)
#include <pwm_z42.h>                // Biblioteca personalizada com funções de controle do TPM (Timer/PWM Module)

// Define o valor do registrador MOD do TPM para configurar o período do PWM
#define TPM_MODULE 1000         // Define a frequência do PWM fpwm = (TPM_CLK / (TPM_MODULE * PS))
// Valores de duty cycle correspondentes a diferentes larguras de pulso
// Duty cycle dos LEDs
#define RED_BASE    (100)   // vermelho 100%
#define GREEN_BASE (36) // verde 36%
#define BLUE_BASE  (TPM_MODULE) // azul 0% --> não entra nas contas é sempre 0


uint16_t duty_red;         
uint16_t duty_green;   
uint16_t duty_blue = BLUE_BASE;  

int intensidade;

uint16_t calcula_duty(uint16_t base, int intensidade)
{
    uint16_t brilho;

    // Mantém a proporção da cor e aplica a intensidade geral
    brilho = (TPM_MODULE * base * intensidade) / (100 * 100);

    // Inverte porque o LED da FRDM-KL25Z é active low
    return TPM_MODULE - brilho;
}

int main(void)
{
    // Inicializa TPM2
    pwm_tpm_Init(TPM2, TPM_OSCERCLK, TPM_MODULE, TPM_CLK, PS_128, EDGE_PWM);

    /*
      FRDM-KL25Z RGB LED:

      PTB18 → Vermelho
      PTB19 → Verde
      PTD1  → Azul
    */

    // Inicializa canais PWM
    pwm_tpm_Ch_Init(TPM2,0,TPM_PWM_H,GPIOB,18); // Vermelho
    pwm_tpm_Ch_Init(TPM2,1,TPM_PWM_H,GPIOB,19); // Verde
    pwm_tpm_Ch_Init(TPM2,2,TPM_PWM_H,GPIOD,1);  // Azul

    while (1)
    {
        printf("\nDigite a intensidade do LED laranja de 0 a 100: ");
        scanf("%d", &intensidade);

        if (intensidade < 0)
        {
            intensidade = 0;
        }

        if (intensidade > 100)
        {
            intensidade = 100;
        }

        duty_red   = calcula_duty(RED_BASE, intensidade);
        duty_green = calcula_duty(GREEN_BASE, intensidade);
        duty_blue  = calcula_duty(BLUE_BASE, intensidade);
    

        pwm_tpm_CnV(TPM2, 0, duty_red);
        pwm_tpm_CnV(TPM2, 1, duty_green);
        pwm_tpm_CnV(TPM2, 2, duty_blue);

        printf("Intensidade aplicada: %d%%\n", intensidade);
    }

    return 0;
}