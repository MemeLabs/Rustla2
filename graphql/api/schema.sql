CREATE TABLE `user_config` (
    `user_id` BIGINT UNSIGNED,
    `name` VARCHAR(16) NOT NULL,
    `value` VARCHAR(2048),
    PRIMARY KEY (`user_id`, `name`),
    FOREIGN KEY (`user_id`)
        REFERENCES `users` (`id`)
        ON DELETE CASCADE,
    CHECK (value IS NULL OR JSON_VALID(value))
) ENGINE=innodb;

CREATE TABLE `users` (
    `id` BIGINT UNSIGNED PRIMARY KEY,
    `created_at` DATETIME NOT NULL,
    `updated_at` DATETIME NOT NULL,
    `twitch_id` VARCHAR(32) NOT NULL,
    `twitch_username` VARCHAR(32) NOT NULL,
    `name` VARCHAR(32) NOT NULL,
    `ban_reason` VARCHAR(512),
    `is_banned` TINYINT(1) NOT NULL DEFAULT 0,
    `is_admin` TINYINT(1) NOT NULL DEFAULT 0,
    UNIQUE KEY (`twitch_id`),
    UNIQUE KEY (`name`)
) ENGINE=innodb;

CREATE TABLE `ip_log` (
    `user_id` BIGINT UNSIGNED,
    `last_seen_at` DATETIME NOT NULL,
    `last_ip` VARCHAR(64) NOT NULL,
    PRIMARY KEY (`user_id`, `last_seen_at`),
    FOREIGN KEY (`user_id`)
        REFERENCES `users` (`id`)
        ON DELETE CASCADE
) ENGINE=innodb;

CREATE TABLE `tags` (
    `id` BIGINT UNSIGNED PRIMARY KEY,
    `value` VARCHAR(64),
    UNIQUE KEY (`value`)
) ENGINE=innodb;

CREATE TABLE `channels` (
    `id` BIGINT UNSIGNED PRIMARY KEY,
    `service` VARCHAR(64),
    `channel` VARCHAR(64),
    UNIQUE KEY (`service`, `channel`)
) ENGINE=innodb;

CREATE TABLE `channel_tags` (
    `channel_id` BIGINT UNSIGNED NOT NULL,
    `tag_id` BIGINT UNSIGNED NOT NULL,
    PRIMARY KEY (`channel_id`, `tag_id`),
    FOREIGN KEY (`channel_id`)
        REFERENCES `channels` (`id`)
        ON DELETE CASCADE,
    FOREIGN KEY (`tag_id`)
        REFERENCES `tags` (`id`)
        ON DELETE CASCADE
) ENGINE=innodb;

CREATE TABLE `service_info` (
    `channel_id` BIGINT UNSIGNED PRIMARY KEY,
    `updated_at` DATETIME NOT NULL,
    `title` VARCHAR(64),
    `thumbnail_url` VARCHAR(1024),
    `is_live` TINYINT(1) NOT NULL DEFAULT 0,
    `viewer_count` INT NOT NULL DEFAULT 0,
    FOREIGN KEY (`channel_id`)
        REFERENCES `channels` (`id`)
        ON DELETE CASCADE
) ENGINE=innodb;

CREATE TABLE `rooms` (
    `id` BIGINT UNSIGNED PRIMARY KEY,
    `path` VARCHAR(32) NOT NULL,
    `owner_user_id` BIGINT UNSIGNED,
    `channel_id` BIGINT UNSIGNED,
    `title` VARCHAR(255),
    `description` TEXT,
    `thumbnail_url` VARCHAR(1024),
    UNIQUE KEY (`path`),
    FOREIGN KEY (`owner_user_id`)
        REFERENCES `users` (`id`)
        ON DELETE SET NULL,
    FOREIGN KEY (`channel_id`)
        REFERENCES `channels` (`id`)
        ON DELETE SET NULL
) ENGINE=innodb;

CREATE TABLE `room_editor_users` (
    `room_id` BIGINT UNSIGNED NOT NULL,
    `user_Id` BIGINT UNSIGNED NOT NULL,
    PRIMARY KEY (`room_id`, `user_id`),
    FOREIGN KEY (`room_id`)
        REFERENCES `rooms` (`id`)
        ON DELETE CASCADE,
    FOREIGN KEY (`user_id`)
        REFERENCES `users` (`id`)
        ON DELETE CASCADE
) ENGINE=innodb;

CREATE TABLE `room_tags` (
    `room_id` BIGINT UNSIGNED,
    `tag_id` BIGINT UNSIGNED,
    PRIMARY KEY (`room_id`, `tag_id`),
    FOREIGN KEY (`room_id`)
        REFERENCES `rooms` (`id`)
        ON DELETE CASCADE,
    FOREIGN KEY (`tag_id`)
        REFERENCES `tags` (`id`)
        ON DELETE CASCADE
) ENGINE=innodb;
