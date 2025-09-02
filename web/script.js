// INGRES ChatBot - Frontend JavaScript

class INGRESChatBot {
    constructor() {
        this.isConnected = false;
        this.isTyping = false;
        this.messageHistory = [];
        
        this.initializeElements();
        this.setupEventListeners();
        this.checkConnection();
        this.showWelcomeMessage();
    }

    initializeElements() {
        this.chatMessages = document.getElementById('chatMessages');
        this.userInput = document.getElementById('userInput');
        this.sendButton = document.getElementById('sendButton');
        this.connectionStatus = document.getElementById('connectionStatus');
        this.statusText = document.getElementById('statusText');
        this.charCount = document.getElementById('charCount');
        this.typingIndicator = document.getElementById('typingIndicator');
        this.loadingOverlay = document.getElementById('loadingOverlay');
    }

    setupEventListeners() {
        // Send message on Enter key
        this.userInput.addEventListener('keypress', (e) => {
            if (e.key === 'Enter' && !e.shiftKey) {
                e.preventDefault();
                this.sendMessage();
            }
        });

        // Character count update
        this.userInput.addEventListener('input', () => {
            const length = this.userInput.value.length;
            this.charCount.textContent = `${length}/500`;
            
            // Enable/disable send button
            this.sendButton.disabled = length === 0 || !this.isConnected;
        });

        // Auto-resize input (if needed for multiline)
        this.userInput.addEventListener('input', () => {
            this.userInput.style.height = 'auto';
            this.userInput.style.height = this.userInput.scrollHeight + 'px';
        });
    }

    async checkConnection() {
        try {
            const response = await fetch('/api/status');
            if (response.ok) {
                this.setConnectionStatus(true);
            } else {
                this.setConnectionStatus(false);
            }
        } catch (error) {
            console.error('Connection check failed:', error);
            this.setConnectionStatus(false);
        }
    }

    setConnectionStatus(connected) {
        this.isConnected = connected;
        
        if (connected) {
            this.connectionStatus.className = 'status-dot online';
            this.statusText.textContent = 'Connected';
            this.sendButton.disabled = this.userInput.value.length === 0;
        } else {
            this.connectionStatus.className = 'status-dot offline';
            this.statusText.textContent = 'Disconnected';
            this.sendButton.disabled = true;
        }
    }

    showWelcomeMessage() {
        const welcomeMessage = {
            type: 'bot',
            content: `🌊 Welcome to INGRES ChatBot!

I'm your AI assistant for groundwater resource information across India. I can help you with:

📊 **Groundwater Data Queries**
• "Show me groundwater data for Punjab"
• "What's the status of Maharashtra's water resources?"

📈 **Historical Analysis**  
• "Compare groundwater trends for Gujarat vs Rajasthan"
• "Show me 5-year data for Karnataka"

⚠️ **Critical Area Identification**
• "Which areas are over-exploited?"
• "Show me safe groundwater zones"

❓ **General Help**
• "Explain groundwater categories"
• "What does stage of extraction mean?"

Try asking me anything about India's groundwater resources!`,
            timestamp: new Date()
        };
        
        this.addMessage(welcomeMessage);
    }

    async sendMessage() {
        const message = this.userInput.value.trim();
        if (!message || !this.isConnected) return;

        // Add user message to chat
        this.addMessage({
            type: 'user',
            content: message,
            timestamp: new Date()
        });

        // Clear input
        this.userInput.value = '';
        this.charCount.textContent = '0/500';
        this.sendButton.disabled = true;

        // Show typing indicator
        this.showTypingIndicator();

        try {
            // Send request to backend
            const response = await fetch('/api/chat', {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/json',
                },
                body: JSON.stringify({
                    message: message,
                    timestamp: new Date().toISOString()
                })
            });

            if (!response.ok) {
                throw new Error(`HTTP error! status: ${response.status}`);
            }

            const data = await response.json();
            
            // Hide typing indicator
            this.hideTypingIndicator();

            // Add bot response
            this.addMessage({
                type: 'bot',
                content: data.response || 'Sorry, I encountered an error processing your request.',
                timestamp: new Date(),
                processingTime: data.processing_time
            });

        } catch (error) {
            console.error('Error sending message:', error);
            this.hideTypingIndicator();
            
            this.addMessage({
                type: 'bot',
                content: '❌ Sorry, I\'m having trouble connecting to the server. Please check your connection and try again.',
                timestamp: new Date(),
                isError: true
            });
        }

        // Re-enable input
        this.sendButton.disabled = false;
    }

    sendQuickQuery(query) {
        this.userInput.value = query;
        this.sendMessage();
    }

    addMessage(message) {
        const messageElement = document.createElement('div');
        messageElement.className = `message ${message.type}`;
        
        const avatar = document.createElement('div');
        avatar.className = 'message-avatar';
        avatar.textContent = message.type === 'user' ? '👤' : '🤖';
        
        const content = document.createElement('div');
        content.className = 'message-content';
        
        // Format message content
        if (message.content.includes('\n')) {
            // Handle multiline messages
            content.innerHTML = this.formatMessage(message.content);
        } else {
            content.textContent = message.content;
        }
        
        const time = document.createElement('div');
        time.className = 'message-time';
        time.textContent = this.formatTime(message.timestamp);
        
        if (message.processingTime) {
            time.textContent += ` • ${message.processingTime}ms`;
        }
        
        messageElement.appendChild(avatar);
        messageElement.appendChild(content);
        content.appendChild(time);
        
        this.chatMessages.appendChild(messageElement);
        this.scrollToBottom();
        
        // Store in history
        this.messageHistory.push(message);
    }

    formatMessage(content) {
        // Simple formatting for bot messages
        return content
            .replace(/\*\*(.*?)\*\*/g, '<strong>$1</strong>')  // Bold
            .replace(/\*(.*?)\*/g, '<em>$1</em>')              // Italic
            .replace(/\n/g, '<br>')                            // Line breaks
            .replace(/•/g, '&bull;');                          // Bullet points
    }

    formatTime(timestamp) {
        return timestamp.toLocaleTimeString('en-US', {
            hour: '2-digit',
            minute: '2-digit'
        });
    }

    showTypingIndicator() {
        this.isTyping = true;
        this.typingIndicator.classList.remove('hidden');
    }

    hideTypingIndicator() {
        this.isTyping = false;
        this.typingIndicator.classList.add('hidden');
    }

    scrollToBottom() {
        this.chatMessages.scrollTop = this.chatMessages.scrollHeight;
    }

    showLoading() {
        this.loadingOverlay.classList.remove('hidden');
    }

    hideLoading() {
        this.loadingOverlay.classList.add('hidden');
    }
}

// Global functions for quick actions
function sendQuickQuery(query) {
    if (window.chatBot) {
        window.chatBot.sendQuickQuery(query);
    }
}

function sendMessage() {
    if (window.chatBot) {
        window.chatBot.sendMessage();
    }
}

// Initialize chatbot when page loads
document.addEventListener('DOMContentLoaded', () => {
    window.chatBot = new INGRESChatBot();
    
    // Periodic connection check
    setInterval(() => {
        window.chatBot.checkConnection();
    }, 30000); // Check every 30 seconds
});

// Handle page visibility changes
document.addEventListener('visibilitychange', () => {
    if (!document.hidden && window.chatBot) {
        window.chatBot.checkConnection();
    }
});