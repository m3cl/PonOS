// Заглушка для stack protector
void __stack_chk_fail(void) {
    // Просто вешаемся если stack corruption
    while(1) {}
}
