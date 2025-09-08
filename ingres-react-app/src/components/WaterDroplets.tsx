import React, { useEffect, useState } from 'react'
import { motion } from 'framer-motion'

interface Droplet {
  id: number
  x: number
  y: number
  size: number
  duration: number
  delay: number
}

const WaterDroplets: React.FC = () => {
  const [droplets, setDroplets] = useState<Droplet[]>([])

  useEffect(() => {
    const generateDroplets = () => {
      const newDroplets: Droplet[] = []
      for (let i = 0; i < 15; i++) {
        newDroplets.push({
          id: i,
          x: Math.random() * 100,
          y: Math.random() * 100,
          size: Math.random() * 8 + 4,
          duration: Math.random() * 3 + 2,
          delay: Math.random() * 2,
        })
      }
      setDroplets(newDroplets)
    }

    generateDroplets()
  }, [])

  return (
    <div className="absolute inset-0 pointer-events-none overflow-hidden">
      {droplets.map((droplet) => (
        <motion.div
          key={droplet.id}
          className="absolute rounded-full bg-gradient-to-br from-cyan-400/30 to-blue-500/20 backdrop-blur-sm"
          style={{
            left: `${droplet.x}%`,
            top: `${droplet.y}%`,
            width: `${droplet.size}px`,
            height: `${droplet.size}px`,
          }}
          animate={{
            scale: [1, 1.2, 1],
            opacity: [0.3, 0.6, 0.3],
            y: [0, -20, 0],
          }}
          transition={{
            duration: droplet.duration,
            delay: droplet.delay,
            repeat: Infinity,
            ease: "easeInOut",
          }}
        />
      ))}
      
      {/* Floating particles */}
      {Array.from({ length: 20 }).map((_, i) => (
        <motion.div
          key={`particle-${i}`}
          className="absolute w-1 h-1 bg-cyan-300/40 rounded-full"
          style={{
            left: `${Math.random() * 100}%`,
            top: `${Math.random() * 100}%`,
          }}
          animate={{
            x: [0, Math.random() * 100 - 50],
            y: [0, Math.random() * 100 - 50],
            opacity: [0, 1, 0],
          }}
          transition={{
            duration: Math.random() * 4 + 3,
            delay: Math.random() * 2,
            repeat: Infinity,
            ease: "linear",
          }}
        />
      ))}
    </div>
  )
}

export default WaterDroplets