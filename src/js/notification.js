let notif_queue = [];

function notif(notif_c) {
    let notification_area = document.querySelector(".statusline .notification-area");

    if (notification_area.querySelector(".notification") === null) {
        let notif_elem = document.createElement("div");
        notif_elem.className = "notification";
        notif_elem.appendChild(notif_c[0]);
        notif_elem.appendChild(notif_c[1]);

        notification_area.appendChild(notif_elem);

        setTimeout(() => {
            notification_area.removeChild(notif_elem);
            if (notif_queue.length > 0) {
                notif(notif_queue.pop());
            }
        }, 5000);
    } else {
        notif_queue.push(notif_c);
    }
}

export function notif_alert(alert_str) {
    let div = document.createElement("div");
    div.className = "notification-content";

    let text = document.createElement("p");
    text.innerHTML = alert_str;
    div.appendChild(text);

    let icon = document.createElement("i");
    icon.className = "alert fas fa-exclamation-triangle";
    div.appendChild(icon);

    notif([icon, div]);
}

export function notif_warn(alert_str) {
    let div = document.createElement("div");
    div.className = "notification-content";

    let text = document.createElement("p");
    text.innerHTML = alert_str;
    div.appendChild(text);

    let icon = document.createElement("i");
    icon.className = "warning fas fa-exclamation-triangle";
    div.appendChild(icon);

    notif([icon, div]);
}

export function notif_info(info_str) {
    let div = document.createElement("div");
    div.className = "notification-content";

    let text = document.createElement("p");
    text.innerHTML = info_str;
    div.appendChild(text);

    let icon = document.createElement("i");
    icon.className = "info fas fa-info-circle";
    div.appendChild(icon);

    notif([icon, div]);
}

export function notif_success(success_str) {
    let div = document.createElement("div");
    div.className = "notification-content";

    let text = document.createElement("p");
    text.innerHTML = success_str;
    div.appendChild(text);

    let icon = document.createElement("i");
    icon.className = "success fas fa-check-circle";
    div.appendChild(icon);

    notif([icon, div]);
}
