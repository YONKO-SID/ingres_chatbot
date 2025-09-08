import React, { useState, useRef, useEffect } from 'react';
import { motion, AnimatePresence } from 'framer-motion';
import { useChatStore } from '../store/chatStore';
import Message from './Message';

const Chat = () => {
  const { messages, addMessage } = useChatStore();
  const [input, setInput] = useState('');
  const [isTyping, setIsTyping] = useState(false);
  const messagesEndRef = useRef<HTMLDivElement>(null);

  const scrollToBottom = () => {
    messagesEndRef.current?.scrollIntoView({ behavior: "smooth" });
  };

  useEffect(() => {
    scrollToBottom();
  }, [messages]);

  const simulateBotResponse = (userMessage: string) => {
    setIsTyping(true);
    
    // Simulate more intelligent responses based on keywords
    const responses = {
      water: "🌊 I can help you with water level information. Which region are you interested in?",
      level: "📊 Current water levels are being monitored across 28 states. Would you like specific data?",
      quality: "💧 Water quality parameters include pH, TDS, and contamination levels. What would you like to know?",
      report: "📋 I can generate comprehensive reports for any region. Please specify the area.",
      help: "🤖 I'm INGRES, your groundwater intelligence assistant. I can help with:\n• Water level monitoring\n• Quality assessment\n• Regional reports\n• Trend analysis\n• Alert notifications",
      default: "I understand you're asking about groundwater. Let me analyze the available data for your query."
    };

    const lowerMessage = userMessage.toLowerCase();
    let response = responses.default;

    for (const [key, value] of Object.entries(responses)) {
      if (key !== 'default' && lowerMessage.includes(key)) {
        response = value;
        break;
      }
    }

    setTimeout(() => {
      setIsTyping(false);
      addMessage({ text: response, sender: 'bot' });
    }, 1500 + Math.random() * 1000);
  };

  const handleSendMessage = () => {
    if (input.trim() === '') return;

    addMessage({ text: input, sender: 'user' });
    simulateBotResponse(input);
    setInput('');
  };

  const quickActions = [
    { text: "Check water levels", icon: "🌊" },
    { text: "Generate report", icon: "📊" },
    { text: "Water quality", icon: "💧" },
    { text: "Help", icon: "❓" },
  ];

  return (
    <div className="w-full h-full flex flex-col">
      {/* Chat Header */}
      <div className="p-4 border-b border-slate-700/50 bg-slate-800/30">
        <div className="flex items-center space-x-3">
          <div className="w-10 h-10 rounded-full bg-gradient-to-r from-cyan-400 to-blue-500 flex items-center justify-center">
            <span className="text-white font-bold">🤖</span>
          </div>
          <div>
            <h3 className="text-white font-semibold">INGRES Assistant</h3>
            <p className="text-xs text-cyan-300">Groundwater Intelligence • Online</p>
          </div>
        </div>
      </div>

      {/* Messages Area */}
      <div className="flex-1 overflow-y-auto p-4 space-y-4">
        <AnimatePresence>
          {messages.length === 0 && (
            <motion.div
              initial={{ opacity: 0, y: 20 }}
              animate={{ opacity: 1, y: 0 }}
              className="text-center py-8"
            >
              <div className="text-6xl mb-4">💧</div>
              <h3 className="text-xl font-semibold text-white mb-2">Welcome to INGRES</h3>
              <p className="text-gray-400 mb-6">Your AI-powered groundwater intelligence assistant</p>
              
              <div className="grid grid-cols-2 gap-2 max-w-md mx-auto">
                {quickActions.map((action, index) => (
                  <motion.button
                    key={action.text}
                    initial={{ opacity: 0, y: 20 }}
                    animate={{ opacity: 1, y: 0 }}
                    transition={{ delay: index * 0.1 }}
                    whileHover={{ scale: 1.05 }}
                    whileTap={{ scale: 0.95 }}
                    onClick={() => setInput(action.text)}
                    className="p-3 bg-slate-700/50 hover:bg-slate-600/50 rounded-lg border border-slate-600/50 hover:border-cyan-500/50 transition-all duration-200"
                  >
                    <div className="text-lg mb-1">{action.icon}</div>
                    <div className="text-xs text-gray-300">{action.text}</div>
                  </motion.button>
                ))}
              </div>
            </motion.div>
          )}

          {messages.map((msg) => (
            <Message key={msg.id} text={msg.text} sender={msg.sender} />
          ))}

          {isTyping && (
            <motion.div
              initial={{ opacity: 0, y: 20 }}
              animate={{ opacity: 1, y: 0 }}
              exit={{ opacity: 0, y: -20 }}
              className="flex items-center space-x-2 text-gray-400"
            >
              <div className="w-8 h-8 rounded-full bg-slate-700 flex items-center justify-center">
                <span className="text-sm">🤖</span>
              </div>
              <div className="bg-slate-700/50 rounded-lg px-4 py-2">
                <div className="flex space-x-1">
                  {[0, 1, 2].map((i) => (
                    <motion.div
                      key={i}
                      className="w-2 h-2 bg-cyan-400 rounded-full"
                      animate={{
                        scale: [1, 1.2, 1],
                        opacity: [0.5, 1, 0.5],
                      }}
                      transition={{
                        duration: 1,
                        delay: i * 0.2,
                        repeat: Infinity,
                      }}
                    />
                  ))}
                </div>
              </div>
            </motion.div>
          )}
        </AnimatePresence>
        <div ref={messagesEndRef} />
      </div>

      {/* Input Area */}
      <div className="p-4 border-t border-slate-700/50 bg-slate-800/30">
        <div className="flex space-x-2">
          <div className="flex-1 relative">
            <input
              type="text"
              placeholder="Ask about groundwater data, quality, or generate reports..."
              className="w-full p-3 pr-12 rounded-lg bg-slate-700/50 border border-slate-600/50 text-white placeholder-gray-400 focus:outline-none focus:border-cyan-500/50 focus:bg-slate-700/70 transition-all duration-200"
              value={input}
              onChange={(e) => setInput(e.target.value)}
              onKeyPress={(e) => e.key === 'Enter' && handleSendMessage()}
            />
            <motion.button
              whileHover={{ scale: 1.1 }}
              whileTap={{ scale: 0.9 }}
              className="absolute right-2 top-1/2 transform -translate-y-1/2 p-2 text-gray-400 hover:text-cyan-400 transition-colors"
            >
              <svg className="w-5 h-5" fill="none" stroke="currentColor" viewBox="0 0 24 24">
                <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={2} d="M19 11a7 7 0 01-7 7m0 0a7 7 0 01-7-7m7 7v4m0 0H8m4 0h4m-4-8a3 3 0 01-3-3V5a3 3 0 116 0v6a3 3 0 01-3 3z" />
              </svg>
            </motion.button>
          </div>
          
          <motion.button
            whileHover={{ scale: 1.05 }}
            whileTap={{ scale: 0.95 }}
            onClick={handleSendMessage}
            disabled={!input.trim()}
            className="px-6 py-3 bg-gradient-to-r from-cyan-500 to-blue-500 hover:from-cyan-600 hover:to-blue-600 disabled:from-gray-600 disabled:to-gray-700 text-white rounded-lg font-medium transition-all duration-200 disabled:cursor-not-allowed"
          >
            <svg className="w-5 h-5" fill="none" stroke="currentColor" viewBox="0 0 24 24">
              <path strokeLinecap="round" strokeLinejoin="round" strokeWidth={2} d="M12 19l9 2-9-18-9 18 9-2zm0 0v-8" />
            </svg>
          </motion.button>
        </div>
        
        <div className="flex items-center justify-between mt-2 text-xs text-gray-500">
          <span>Press Enter to send</span>
          <span>Powered by INGRES AI</span>
        </div>
      </div>
    </div>
  );
};

export default Chat;