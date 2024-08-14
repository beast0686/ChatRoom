const socket = io();

socket.on('response', function(data) {
    const message = data.message;
    const messagesDiv = document.getElementById('messages');
    const p = document.createElement('p');
    p.className = message.includes('<You>') ? 'self' : 'other';
    p.textContent = message;
    messagesDiv.appendChild(p);
    messagesDiv.scrollTop = messagesDiv.scrollHeight; // Auto-scroll to the bottom
});

document.getElementById('message-form').addEventListener('submit', function(event) {
    event.preventDefault();
    const input = document.getElementById('message-input');
    const message = input.value;
    if (message) {
        socket.emit('message', { message: message });
        input.value = '';
    }
});
