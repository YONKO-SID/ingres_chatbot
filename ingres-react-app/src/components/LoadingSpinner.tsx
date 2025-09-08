import React from 'react'
import { motion } from 'framer-motion'

const LoadingSpinner: React.FC = () => {
  return (
    <div className="absolute inset-0 z-50 flex items-center justify-center bg-slate-900/80 backdrop-blur-sm">
      <div className="text-center">
        {/* Main Spinner */}
        <motion.div
          className="relative w-20 h-20 mx-auto mb-4"
          animate={{ rotate: 360 }}
          transition={{ duration: 2, repeat: Infinity, ease: "linear" }}
        >
          <div className="absolute inset-0 border-4 border-cyan-500/30 rounded-full"></div>
          <div className="absolute inset-0 border-4 border-transparent border-t-cyan-400 rounded-full"></div>
        </motion.div>

        {/* Water Wave Animation */}
        <div className="relative w-16 h-16 mx-auto mb-4 overflow-hidden rounded-full bg-slate-800">
          <motion.div
            className="absolute bottom-0 left-0 right-0 bg-gradient-to-t from-cyan-400 to-blue-500"
            animate={{
              height: ["20%", "80%", "20%"],
            }}
            transition={{
              duration: 2,
              repeat: Infinity,
              ease: "easeInOut",
            }}
          />
          <motion.div
            className="absolute inset-0 bg-gradient-to-r from-transparent via-white/20 to-transparent"
            animate={{
              x: ["-100%", "100%"],
            }}
            transition={{
              duration: 1.5,
              repeat: Infinity,
              ease: "linear",
            }}
          />
        </div>

        {/* Loading Text */}
        <motion.div
          initial={{ opacity: 0 }}
          animate={{ opacity: 1 }}
          className="text-cyan-300"
        >
          <h3 className="text-lg font-semibold mb-2">Initializing INGRES</h3>
          <motion.p
            className="text-sm text-gray-400"
            animate={{ opacity: [0.5, 1, 0.5] }}
            transition={{ duration: 1.5, repeat: Infinity }}
          >
            Connecting to groundwater database...
          </motion.p>
        </motion.div>

        {/* Progress Dots */}
        <div className="flex justify-center space-x-2 mt-4">
          {[0, 1, 2].map((i) => (
            <motion.div
              key={i}
              className="w-2 h-2 bg-cyan-400 rounded-full"
              animate={{
                scale: [1, 1.5, 1],
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
    </div>
  )
}

export default LoadingSpinner