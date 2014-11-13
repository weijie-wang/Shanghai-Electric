/******************GPIO TASK******************/
Task_Handle tsk_gpio;
void gpio_task(UArg arg0, UArg arg1)
{
    int i = 0;

    for(i = 0; i < 10000; i++)
    {
        Task_sleep (10);
        HWREG(GPIO8OUT) = ~HWREG(GPIO8OUT);

    }
}
void init_gpio()
{
    Task_Params taskParams;

    HWREG(PINMUX18) &= ~(0xf<<20);
    HWREG(PINMUX18) |= (0x8<<20);
    HWREG(GPIO8DIR) = 0;//0 for output
    HWREG(GPIO8OUT) = (0);

    Task_Params_init(&taskParams);
    taskParams.priority = 4;

    tsk_gpio = Task_create (gpio_task, &taskParams, NULL);
}
