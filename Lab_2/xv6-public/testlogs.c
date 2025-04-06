

#include "types.h"

#include "user.h"

int main() {
    // Create two users
    if (make_user_syscall(1, "pass") == 0)
        printf(1, "User 1 created.\n");
    else
        printf(1, "User 1 creation failed.\n");

    if (make_user_syscall(2, "1234") == 0)
        printf(1, "User 2 created.\n");
    else
        printf(1, "User 2 creation failed.\n");

    // Try to create a user with duplicate ID
    if (make_user_syscall(1, "newpass") == -1)
        printf(1, "Duplicate User ID check passed.\n");

    // Attempt login with wrong password
    if (login_syscall(1, "wrong") == -1)
        printf(1, "Wrong password login test passed.\n");

    // Login with correct password
    if (login_syscall(1, "pass") == 0)
        printf(1, "User 1 logged in.\n");

    // Trigger some syscalls
    getpid();

    // Logs while logged in (should show only user 1 logs)
    printf(1, "\nLogs for logged in user (User 1):\n");
    get_logs_syscall();

    // Logout
    if (logout_syscall() == 0)
        printf(1, "User 1 logged out.\n");

    // Attempt to logout again (no user logged in)
    if (logout_syscall() == -1)
        printf(1, "Second logout without login failed as expected.\n");

    // Logs after logout (should show logs of all users)
    printf(1, "\nLogs with no user logged in (all logs):\n");
    get_logs_syscall();

    // Login as user 2
    if (login_syscall(2, "1234") == 0)
        printf(1, "User 2 logged in.\n");

    getpid();

    printf(1, "\nUser 2 logs:\n");
    get_logs_syscall();

    logout_syscall();
    exit();
}

