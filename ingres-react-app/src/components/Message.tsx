import React from 'react';
import { motion } from 'framer-motion';

interface MessageProps {
  text: string;
  sender: 'user' | 'bot';
}

const Message: React.FC<MessageProps> = ({ text, sender }) => {
  const isUser = sender === 'user';
  
  return (
    <motion.div
      initial={{ opacity: 0, y: 20, scale: 0.95 }}
      animate={{ opacity: 1, y: 0, scale: 1 }}
      transition={{ duration: 0.3 }}
      className={`flex ${isUser ? 'justify-end' : 'justify-start'} mb-4`}
    >
      <div className={`flex items-start space-x-2 max-w-[80%] ${isUser ? 'flex-row-reverse space-x-reverse' : ''}`}>
        {/* Avatar */}
        <div className={`w-8 h-8 rounded-full flex items-center justify-center flex-shrink-0 ${
          isUser 
            ? 'bg-gradient-to-r from-blue-500 to-purple-500' 
            : 'bg-gradient-to-r from-cyan-400 to-blue-500'
        }`}>
          <span className="text-white text-sm">
            {isUser ? '👤' : '🤖'}
          </span>
        </div>

        {/* Message Bubble */}
        <motion.div
          whileHover={{ scale: 1.02 }}
          className={`relative px-4 py-3 rounded-2xl shadow-lg ${
            isUser
              ? 'bg-gradient-to-r from-blue-500 to-purple-500 text-white'
              : 'bg-slate-700/80 backdrop-blur-sm text-gray-100 border border-slate-600/50'
          }`}
        >
          {/* Message Text */}
          <div className="whitespace-pre-wrap text-sm leading-relaxed">
            {text}
          </div>

          {/* Message Tail */}
          <div className={`absolute top-3 w-3 h-3 transform rotate-45 ${
            isUser
              ? 'right-[-6px] bg-gradient-to-r from-blue-500 to-purple-500'
              : 'left-[-6px] bg-slate-700/80 border-l border-b border-slate-600/50'
          }`} />

          {/* Timestamp */}
          <div className={`text-xs mt-1 opacity-70 ${
            isUser ? 'text-blue-100' : 'text-gray-400'
          }`}>
            {new Date().toLocaleTimeString([], { hour: '2-digit', minute: '2-digit' })}
          </div>
        </motion.div>
      </div>
    </motion.div>
  );
};

export default Message;
