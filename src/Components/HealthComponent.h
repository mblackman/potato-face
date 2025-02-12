#pragma once

struct HealthComponent {
    int currentHealth;
    int maxHealth;

    HealthComponent(int maxHealth = 0) : currentHealth(maxHealth), maxHealth(maxHealth) {
    }
};
