import { useState } from 'react';
import './App.css';
import { ChatbotPage } from './pages/ChatbotPage';

function App() {
  const [showChatbot, setShowChatbot] = useState(false);
  const [currentLanguage, setCurrentLanguage] = useState('en');
  const [chatbotQuery, setChatbotQuery] = useState('');

  const translations = {
    en: {
      title: "INGRES Groundwater Intelligence",
      subtitle: "Advanced AI-Powered Groundwater Assessment & Analysis Platform",
      description: "Smart India Hackathon 2025 - Revolutionary groundwater monitoring and prediction system",
      chatbotButton: "🤖 Open AI Assistant",
      keyFeatures: "Key Features",
      features: [
        {
          icon: "🧠",
          title: "Intelligent Query Handling",
          description: "Advanced AI for groundwater estimation data analysis with natural language processing"
        },
        {
          icon: "⚡",
          title: "Real-time Assessment",
          description: "Live access to current and historical groundwater assessment results across India"
        },
        {
          icon: "📊",
          title: "Interactive Visualizations",
          description: "Scientific diagrams, charts, and 3D models for comprehensive data understanding"
        },
        {
          icon: "🌐",
          title: "Multilingual Support",
          description: "Available in Hindi, English, Tamil, Telugu, Bengali, and 15+ Indian languages"
        },
        {
          icon: "🔗",
          title: "INGRES Integration",
          description: "Seamless connection to IIT Hyderabad's INGRES database for instant information retrieval"
        }
      ],
      quickActions: "Quick Actions",
      actions: ["📊 Real-time Data", "📄 PDF Analysis", "🔍 Critical Areas", "📈 Trend Analysis", "🗺️ State Reports", "💧 Quality Assessment"]
    },
    hi: {
      title: "इंग्रेस भूजल बुद्धिमत्ता",
      subtitle: "उन्नत AI-संचालित भूजल मूल्यांकन और विश्लेषण प्लेटफॉर्म",
      description: "स्मार्ट इंडिया हैकथॉन 2025 - क्रांतिकारी भूजल निगरानी और पूर्वानुमान प्रणाली",
      chatbotButton: "🤖 AI सहायक खोलें",
      keyFeatures: "मुख्य विशेषताएं",
      features: [
        {
          icon: "🧠",
          title: "बुद्धिमान प्रश्न प्रबंधन",
          description: "प्राकृतिक भाषा प्रसंस्करण के साथ भूजल अनुमान डेटा विश्लेषण के लिए उन्नत AI"
        },
        {
          icon: "⚡",
          title: "वास्तविक समय मूल्यांकन",
          description: "भारत भर में वर्तमान और ऐतिहासिक भूजल मूल्यांकन परिणामों तक लाइव पहुंच"
        },
        {
          icon: "📊",
          title: "इंटरैक्टिव विज़ुअलाइज़ेशन",
          description: "व्यापक डेटा समझ के लिए वैज्ञानिक आरेख, चार्ट और 3D मॉडल"
        },
        {
          icon: "🌐",
          title: "बहुभाषी समर्थन",
          description: "हिंदी, अंग्रेजी, तमिल, तेलुगु, बंगाली और 15+ भारतीय भाषाओं में उपलब्ध"
        },
        {
          icon: "🔗",
          title: "इंग्रेस एकीकरण",
          description: "तत्काल जानकारी पुनर्प्राप्ति के लिए IIT हैदराबाद के इंग्रेस डेटाबेस से निर्बाध कनेक्शन"
        }
      ],
      quickActions: "त्वरित कार्य",
      actions: ["📊 वास्तविक समय डेटा", "📄 PDF विश्लेषण", "🔍 महत्वपूर्ण क्षेत्र", "📈 प्रवृत्ति विश्लेषण", "🗺️ राज्य रिपोर्ट", "💧 गुणवत्ता मूल्यांकन"]
    },
    ta: {
      title: "இங்க்ரெஸ் நிலத்தடி நீர் புலனாய்வு",
      subtitle: "மேம்பட்ட AI-இயங்கும் நிலத்தடி நீர் மதிப்பீடு மற்றும் பகுப்பாய்வு தளம்",
      description: "ஸ்மார்ட் இந்தியா ஹேக்கத்தான் 2025 - புரட்சிகர நிலத்தடி நீர் கண்காணிப்பு மற்றும் முன்னறிவிப்பு அமைப்பு",
      chatbotButton: "🤖 AI உதவியாளரைத் திறக்கவும்",
      keyFeatures: "முக்கிய அம்சங்கள்",
      features: [
        {
          icon: "🧠",
          title: "புத்திசாலித்தனமான வினவல் கையாளுதல்",
          description: "இயற்கை மொழி செயலாக்கத்துடன் நிலத்தடி நீர் மதிப்பீட்டு தரவு பகுப்பாய்விற்கான மேம்பட்ட AI"
        },
        {
          icon: "⚡",
          title: "நிகழ்நேர மதிப்பீடு",
          description: "இந்தியா முழுவதும் தற்போதைய மற்றும் வரலாற்று நிலத்தடி நீர் மதிப்பீட்டு முடிவுகளுக்கான நேரடி அணுகல்"
        },
        {
          icon: "📊",
          title: "ஊடாடும் காட்சிப்படுத்தல்கள்",
          description: "விரிவான தரவு புரிதலுக்கான அறிவியல் வரைபடங்கள், விளக்கப்படங்கள் மற்றும் 3D மாதிரிகள்"
        },
        {
          icon: "🌐",
          title: "பல மொழி ஆதரவு",
          description: "தமிழ், ஆங்கிலம், ஹிந்தி, தெலுங்கு, வங்காளம் மற்றும் 15+ இந்திய மொழிகளில் கிடைக்கிறது"
        },
        {
          icon: "🔗",
          title: "இங்க்ரெஸ் ஒருங்கிணைப்பு",
          description: "உடனடி தகவல் மீட்டெடுப்பிற்காக IIT ஹைதராபாத்தின் இங்க்ரெஸ் தரவுத்தளத்துடன் தடையற்ற இணைப்பு"
        }
      ],
      quickActions: "விரைவு செயல்கள்",
      actions: ["📊 நிகழ்நேர தரவு", "📄 PDF பகுப்பாய்வு", "🔍 முக்கியமான பகுதிகள்", "📈 போக்கு பகுப்பாய்வு", "🗺️ மாநில அறிக்கைகள்", "💧 தரம் மதிப்பீடு"]
    }
  };

  const currentTranslation = translations[currentLanguage as keyof typeof translations];

  const handleQuickAction = (action: string) => {
    setChatbotQuery(action);
    setShowChatbot(true);
  };

  if (showChatbot) {
    return <ChatbotPage initialQuery={chatbotQuery} />;
  }

  return (
    <div style={{ 
      position: 'relative', 
      minHeight: '100vh', 
      background: 'linear-gradient(135deg, #0c0c0c 0%, #1a1a2e 50%, #16213e 100%)',
      color: 'white',
      overflow: 'hidden'
    }}>
      {/* Animated Background */}
      <div style={{
        position: 'absolute',
        top: 0,
        left: 0,
        right: 0,
        bottom: 0,
        background: `radial-gradient(circle at 20% 80%, rgba(79, 172, 254, 0.1) 0%, transparent 50%),
                     radial-gradient(circle at 80% 20%, rgba(79, 172, 254, 0.1) 0%, transparent 50%),
                     radial-gradient(circle at 40% 40%, rgba(79, 172, 254, 0.1) 0%, transparent 50%)`,
        animation: 'float 8s ease-in-out infinite',
        zIndex: 0
      }} />

      {/* Language Selector */}
      <div style={{
        position: 'fixed',
        top: '20px',
        left: '20px',
        zIndex: 1000,
        display: 'flex',
        gap: '8px'
      }}>
        {[
          { code: 'en', name: 'EN', flag: '🇺🇸' },
          { code: 'hi', name: 'हि', flag: '🇮🇳' },
          { code: 'ta', name: 'த', flag: '🇮🇳' }
        ].map((lang) => (
          <button
            key={lang.code}
            onClick={() => setCurrentLanguage(lang.code)}
            style={{
              background: currentLanguage === lang.code 
                ? 'linear-gradient(135deg, #4facfe 0%, #00f2fe 100%)'
                : 'rgba(255, 255, 255, 0.1)',
              backdropFilter: 'blur(20px)',
              WebkitBackdropFilter: 'blur(20px)',
              color: 'white',
              border: `1px solid ${currentLanguage === lang.code ? '#4facfe' : 'rgba(255,255,255,0.2)'}`,
              padding: '8px 12px',
              borderRadius: '20px',
              cursor: 'pointer',
              fontSize: '12px',
              fontWeight: 'bold',
              display: 'flex',
              alignItems: 'center',
              gap: '4px',
              transition: 'all 0.3s ease',
              transform: currentLanguage === lang.code ? 'scale(1.05)' : 'scale(1)'
            }}
          >
            <span>{lang.flag}</span>
            <span>{lang.name}</span>
          </button>
        ))}
      </div>

      {/* Chatbot Access Button */}
      <button
        onClick={() => setShowChatbot(true)}
        style={{
          position: 'fixed',
          top: '20px',
          right: '20px',
          zIndex: 1000,
          background: 'linear-gradient(135deg, #667eea 0%, #764ba2 100%)',
          color: 'white',
          border: 'none',
          padding: '12px 20px',
          borderRadius: '25px',
          cursor: 'pointer',
          fontSize: '14px',
          fontWeight: 'bold',
          boxShadow: '0 4px 15px rgba(102, 126, 234, 0.4)',
          transition: 'all 0.3s ease',
          transform: 'translateY(0)'
        }}
        onMouseEnter={(e) => {
          e.currentTarget.style.transform = 'translateY(-2px)';
          e.currentTarget.style.boxShadow = '0 8px 25px rgba(102, 126, 234, 0.6)';
        }}
        onMouseLeave={(e) => {
          e.currentTarget.style.transform = 'translateY(0)';
          e.currentTarget.style.boxShadow = '0 4px 15px rgba(102, 126, 234, 0.4)';
        }}
      >
        {currentTranslation.chatbotButton}
      </button>
      
      {/* Hero Section */}
      <div style={{
        display: 'flex',
        flexDirection: 'column',
        alignItems: 'center',
        justifyContent: 'center',
        minHeight: '100vh',
        textAlign: 'center',
        padding: '40px 20px',
        position: 'relative',
        zIndex: 5
      }}>
        <h1 style={{ 
          fontSize: '3.5rem', 
          marginBottom: '1rem',
          background: 'linear-gradient(135deg, #667eea 0%, #764ba2 100%)',
          WebkitBackgroundClip: 'text',
          WebkitTextFillColor: 'transparent',
          backgroundClip: 'text',
          fontWeight: 'bold',
          textShadow: '0 0 30px rgba(102, 126, 234, 0.5)',
          animation: 'glow 2s ease-in-out infinite alternate',
          lineHeight: '1.2'
        }}>
          🌊 {currentTranslation.title}
        </h1>
        
        <p style={{ 
          fontSize: '1.3rem', 
          marginBottom: '1rem', 
          opacity: 0.9,
          background: 'linear-gradient(135deg, #f093fb 0%, #f5576c 100%)',
          WebkitBackgroundClip: 'text',
          WebkitTextFillColor: 'transparent',
          backgroundClip: 'text',
          fontWeight: '500',
          maxWidth: '800px'
        }}>
          {currentTranslation.subtitle}
        </p>
        
        <p style={{ 
          fontSize: '1rem', 
          marginBottom: '3rem', 
          opacity: 0.7,
          maxWidth: '600px'
        }}>
          {currentTranslation.description}
        </p>

        {/* Key Features Section */}
        <div style={{
          width: '100%',
          maxWidth: '1200px',
          marginBottom: '3rem'
        }}>
          <h2 style={{
            fontSize: '2.5rem',
            marginBottom: '2rem',
            background: 'linear-gradient(135deg, #4facfe 0%, #00f2fe 100%)',
            WebkitBackgroundClip: 'text',
            WebkitTextFillColor: 'transparent',
            backgroundClip: 'text',
            fontWeight: 'bold'
          }}>
            {currentTranslation.keyFeatures}
          </h2>
          
          <div style={{
            display: 'grid',
            gridTemplateColumns: 'repeat(auto-fit, minmax(350px, 1fr))',
            gap: '25px',
            padding: '0 20px'
          }}>
            {currentTranslation.features.map((feature, i) => (
              <button
                key={i}
                onClick={() => {
                  const featureQueries = {
                    0: 'Intelligent query handling',
                    1: 'Real-time assessment',
                    2: 'Interactive visualizations',
                    3: 'Multilingual support',
                    4: 'INGRES integration'
                  };
                  handleQuickAction(featureQueries[i as keyof typeof featureQueries] || 'INGRES features');
                }}
                style={{
                  background: 'rgba(255, 255, 255, 0.1)',
                  backdropFilter: 'blur(20px)',
                  WebkitBackdropFilter: 'blur(20px)',
                  border: '1px solid rgba(255, 255, 255, 0.1)',
                  borderRadius: '20px',
                  padding: '30px',
                  textAlign: 'left',
                  transition: 'all 0.3s ease',
                  transform: 'translateY(0)',
                  boxShadow: '0 8px 32px rgba(0, 0, 0, 0.3)',
                  cursor: 'pointer',
                  color: 'white',
                  fontSize: 'inherit',
                  fontFamily: 'inherit',
                  width: '100%'
                }}
                onMouseEnter={(e) => {
                  e.currentTarget.style.transform = 'translateY(-5px)';
                  e.currentTarget.style.boxShadow = '0 15px 40px rgba(79, 172, 254, 0.2)';
                  e.currentTarget.style.border = '1px solid rgba(79, 172, 254, 0.3)';
                }}
                onMouseLeave={(e) => {
                  e.currentTarget.style.transform = 'translateY(0)';
                  e.currentTarget.style.boxShadow = '0 8px 32px rgba(0, 0, 0, 0.3)';
                  e.currentTarget.style.border = '1px solid rgba(255, 255, 255, 0.1)';
                }}
              >
                <div style={{
                  fontSize: '3rem',
                  marginBottom: '15px',
                  textAlign: 'center'
                }}>
                  {feature.icon}
                </div>
                <h3 style={{
                  fontSize: '1.3rem',
                  marginBottom: '12px',
                  color: '#4facfe',
                  fontWeight: 'bold'
                }}>
                  {feature.title}
                </h3>
                <p style={{
                  fontSize: '1rem',
                  lineHeight: '1.6',
                  opacity: 0.8,
                  margin: 0
                }}>
                  {feature.description}
                </p>
              </button>
            ))}
          </div>
        </div>

        {/* Quick Actions */}
        <div style={{
          width: '100%',
          maxWidth: '1000px'
        }}>
          <h3 style={{
            fontSize: '1.8rem',
            marginBottom: '1.5rem',
            color: '#4facfe',
            fontWeight: 'bold'
          }}>
            {currentTranslation.quickActions}
          </h3>
          
          <div style={{ 
            display: 'flex', 
            gap: '15px', 
            flexWrap: 'wrap', 
            justifyContent: 'center'
          }}>
            {currentTranslation.actions.map((action, i) => (
              <button
                key={i}
                onClick={() => handleQuickAction(action)}
                style={{ 
                  background: 'rgba(79, 172, 254, 0.2)', 
                  backdropFilter: 'blur(20px)',
                  WebkitBackdropFilter: 'blur(20px)',
                  color: 'white', 
                  padding: '12px 20px', 
                  borderRadius: '25px',
                  border: '1px solid rgba(79, 172, 254, 0.3)',
                  cursor: 'pointer',
                  transition: 'all 0.3s ease',
                  transform: 'translateY(0)',
                  boxShadow: '0 4px 15px rgba(0, 0, 0, 0.2)',
                  fontSize: '14px',
                  fontWeight: '500'
                }}
                onMouseEnter={(e) => {
                  e.currentTarget.style.transform = 'translateY(-2px)';
                  e.currentTarget.style.background = 'rgba(79, 172, 254, 0.3)';
                  e.currentTarget.style.boxShadow = '0 8px 25px rgba(79, 172, 254, 0.4)';
                }}
                onMouseLeave={(e) => {
                  e.currentTarget.style.transform = 'translateY(0)';
                  e.currentTarget.style.background = 'rgba(79, 172, 254, 0.2)';
                  e.currentTarget.style.boxShadow = '0 4px 15px rgba(0, 0, 0, 0.2)';
                }}
              >
                {action}
              </button>
            ))}
          </div>
        </div>
      </div>

      {/* CSS Animations */}
      <style>{`
        @keyframes float {
          0%, 100% {
            transform: translateY(0px);
          }
          50% {
            transform: translateY(-20px);
          }
        }
        
        @keyframes glow {
          from {
            text-shadow: 0 0 20px rgba(102, 126, 234, 0.5);
          }
          to {
            text-shadow: 0 0 30px rgba(102, 126, 234, 0.8), 0 0 40px rgba(102, 126, 234, 0.6);
          }
        }
      `}</style>
    </div>
  );
}

export default App;
