#if !defined(CONSOLE_H)
    #define CONSOLE_H 
    #include <cstdio>

    #define CRED printf("\033[31m");
    #define CORANGE printf("\033[38;5;202m");
    #define CGREEN printf("\033[32m");
    #define CDEFAULT printf("\033[39m");

    #define PRINTERROR(...) CRED printf("ERROR: "); printf(__VA_ARGS__); CDEFAULT printf("\n");
    #define PRINTWARN(...) CORANGE printf("WARN: "); printf(__VA_ARGS__); CDEFAULT printf("\n");
    #define PRINTSUCCESS(...) CGREEN printf("SUCCESS: "); printf(__VA_ARGS__); CDEFAULT printf("\n");

    #ifdef DEBUG
        #define PRINTDBG(...) printf("DEBUG: "); printf(__VA_ARGS__);
    #else
        #define PRINTDBG(...)
    #endif

#endif