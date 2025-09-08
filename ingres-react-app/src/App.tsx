import { Canvas } from '@react-three/fiber'
import { Suspense, useState } from 'react'
import Scene from './components/Scene'
import GlareHover from './components/GlareHover'
import Chat from './components/Chat'
import Header from './components/Header'
import Sidebar from './components/Sidebar'
import WaterDroplets from './components/WaterDroplets'
import LoadingSpinner from './components/LoadingSpinner'
import StatsPanel from './components/StatsPanel'
import FloatingActionButton from './components/FloatingActionButton'
import NotificationSystem from './components/NotificationSystem'
import { motion, AnimatePresence } from 'framer-motion'

function App() {
  const [sidebarOpen, setSidebarOpen] = useState(false)
  const [activeView, setActiveView] = useState<'chat' | 'analytics' | 'map'>('chat')

  return (
    <div className="relative w-full h-screen bg-gradient-to-br from-blue-900 via-blue-800 to-cyan-900 overflow-hidden">
      {/* Animated Water Droplets Background */}
      <WaterDroplets />
      
      {/* 3D Scene Background */}
      <div className="absolute inset-0 z-0">
        <Canvas camera={{ position: [0, 5, 10], fov: 60 }}>
          <Suspense fallback={null}>
            <ambientLight intensity={0.4} />
            <pointLight position={[10, 10, 10]} intensity={0.8} color="#4FC3F7" />
            <pointLight position={[-10, -10, -5]} intensity={0.3} color="#81C784" />
            <Scene />
          </Suspense>
        </Canvas>
      </div>

      {/* Header */}
      <Header onMenuClick={() => setSidebarOpen(!sidebarOpen)} />

      {/* Sidebar */}
      <AnimatePresence>
        {sidebarOpen && (
          <Sidebar 
            onClose={() => setSidebarOpen(false)}
            activeView={activeView}
            onViewChange={setActiveView}
          />
        )}
      </AnimatePresence>

      {/* Main Content Area */}
      <div className="absolute inset-0 z-10 pt-16 flex">
        {/* Stats Panel */}
        <motion.div
          initial={{ x: -300, opacity: 0 }}
          animate={{ x: 0, opacity: 1 }}
          transition={{ delay: 0.5, duration: 0.8 }}
          className="hidden lg:block w-80 p-4"
        >
          <StatsPanel />
        </motion.div>

        {/* Chat Interface */}
        <div className="flex-1 flex items-center justify-center p-4">
          <motion.div
            initial={{ scale: 0.8, opacity: 0 }}
            animate={{ scale: 1, opacity: 1 }}
            transition={{ duration: 0.8 }}
            className="w-full max-w-4xl h-full max-h-[80vh]"
          >
            <GlareHover
              width="100%"
              height="100%"
              background="rgba(15, 23, 42, 0.85)"
              borderRadius="16px"
              borderColor="rgba(59, 130, 246, 0.3)"
              glareColor="#4FC3F7"
              glareOpacity={0.15}
              className="backdrop-blur-md border border-blue-500/30 shadow-2xl"
            >
              <Chat />
            </GlareHover>
          </motion.div>
        </div>

        {/* Quick Actions Panel */}
        <motion.div
          initial={{ x: 300, opacity: 0 }}
          animate={{ x: 0, opacity: 1 }}
          transition={{ delay: 0.7, duration: 0.8 }}
          className="hidden xl:block w-64 p-4"
        >
          <div className="bg-slate-800/60 backdrop-blur-md rounded-xl p-4 border border-cyan-500/30">
            <h3 className="text-cyan-300 font-semibold mb-4">Quick Actions</h3>
            <div className="space-y-3">
              {[
                { icon: "🌊", text: "Water Level Check", color: "blue" },
                { icon: "📊", text: "Generate Report", color: "green" },
                { icon: "🗺️", text: "View Map", color: "purple" },
                { icon: "⚠️", text: "Alert Status", color: "red" },
              ].map((action, index) => (
                <motion.button
                  key={action.text}
                  whileHover={{ scale: 1.05, x: 5 }}
                  whileTap={{ scale: 0.95 }}
                  className={`w-full p-3 rounded-lg bg-${action.color}-500/20 border border-${action.color}-500/30 text-${action.color}-300 hover:bg-${action.color}-500/30 transition-all duration-200 flex items-center space-x-3`}
                >
                  <span className="text-xl">{action.icon}</span>
                  <span className="text-sm">{action.text}</span>
                </motion.button>
              ))}
            </div>
          </div>
        </motion.div>
      </div>

      {/* Floating Action Button */}
      <FloatingActionButton />

      {/* Notification System */}
      <NotificationSystem />

      {/* Loading Overlay */}
      <Suspense fallback={<LoadingSpinner />}>
        <div />
      </Suspense>
    </div>
  )
}

export default App