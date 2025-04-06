#include "types.h"
#include "user.h"

int main() {
    // Try to create a user
    if (make_user_syscall(1, "pass") == 0)
        printf(1, "User 1 created successfully.\n");
    else
        printf(1, "User creation failed due to duplicate ID.\n");

    // Try logging in
    if (login_syscall(1, "pass") == 0)
        printf(1, "User 1 logged in.\n");
    else
        printf(1, "Login failed.\n");

    // Trigger a syscall like getpid to log something
    getpid();

    // Show logs
    get_logs_syscall();

    // Logout
    logout_syscall();

    exit();
}
