#include <Windows.h>
#include <iostream>
#include "API_Pause.h"
#include "ini_config.h"
#include "doukutsu/all.h"
#include "SDL_Pause.h"
#include "Backend.h"
#include "File.h"
#include "game_hooks.h"

void writeToLog(const char* format, ...) {
	// Open the log file in append mode
	FILE* file = fopen("log.txt", "a");
	if (file != NULL) {
		// Use variable arguments to handle printf-style formatting
		va_list args;
		va_start(args, format);
		// Write formatted message to the file
		vfprintf(file, format, args);
		va_end(args);
		// Close the file
		fclose(file);
	}
	else {
		// Failed to open the file
		printf("Error: Unable to open log file.\n");
	}
}