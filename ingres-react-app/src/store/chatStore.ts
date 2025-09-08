import {create} from 'zustand';

interface Message {
  id: number;
  text: string;
  sender: 'user' | 'bot';
  timestamp: Date;
  type?: 'text' | 'data' | 'chart' | 'alert';
  metadata?: {
    location?: string;
    dataType?: string;
    confidence?: number;
  };
}

interface ChatState {
  messages: Message[];
  isConnected: boolean;
  currentSession: string;
  addMessage: (message: Omit<Message, 'id' | 'timestamp'>) => void;
  clearMessages: () => void;
  setConnectionStatus: (status: boolean) => void;
  startNewSession: () => void;
}

export const useChatStore = create<ChatState>((set, get) => ({
  messages: [],
  isConnected: true,
  currentSession: `session_${Date.now()}`,
  
  addMessage: (message) =>
    set((state) => ({
      messages: [...state.messages, { 
        ...message, 
        id: state.messages.length,
        timestamp: new Date()
      }],
    })),
    
  clearMessages: () =>
    set(() => ({
      messages: [],
    })),
    
  setConnectionStatus: (status) =>
    set(() => ({
      isConnected: status,
    })),
    
  startNewSession: () =>
    set(() => ({
      currentSession: `session_${Date.now()}`,
      messages: [],
    })),
}));
