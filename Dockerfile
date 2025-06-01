# Base image with GCC pre-installed
FROM gcc:latest

# Set working directory inside the container
WORKDIR /app

# Copy all source files into the container
COPY scheduler.c scheduler.h scheduler_test.c ./

# Compile the test file
RUN gcc -Wall -Wextra -std=c11 -o scheduler_test scheduler_test.c scheduler.c -lm

# Default command: run the test binary
CMD ["./scheduler_test"]
