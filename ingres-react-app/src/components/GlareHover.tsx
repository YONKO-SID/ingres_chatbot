import React, { useRef, useState } from "react";
import "./GlareHover.css";

interface GlareHoverProps {
  width?: string;
  height?: string;
  background?: string;
  borderRadius?: string;
  borderColor?: string;
  children?: React.ReactNode;
  glareColor?: string;
  glareOpacity?: number;
  glareSize?: number;
  transitionDuration?: number;
  playOnce?: boolean;
  className?: string;
  style?: React.CSSProperties;
}

const GlareHover: React.FC<GlareHoverProps> = ({
  width = "500px",
  height = "500px",
  background = "#000",
  borderRadius = "10px",
  borderColor = "#333",
  children,
  glareColor = "#ffffff",
  glareOpacity = 0.5,
  glareSize = 250,
  transitionDuration = 650,
  playOnce = false,
  className = "",
  style = {},
}) => {
  const ref = useRef<HTMLDivElement>(null);
  const [isHovering, setIsHovering] = useState(false);

  const handleMouseMove = (e: React.MouseEvent<HTMLDivElement>) => {
    if (ref.current) {
      const { left, top, width, height } = ref.current.getBoundingClientRect();
      const x = e.clientX - left;
      const y = e.clientY - top;
      ref.current.style.setProperty("--mouse-x", `${x}px`);
      ref.current.style.setProperty("--mouse-y", `${y}px`);
    }
  };

  const handleMouseEnter = () => {
    setIsHovering(true);
  };

  const handleMouseLeave = () => {
    if (!playOnce) {
      setIsHovering(false);
    }
  };

  const glareStyle: React.CSSProperties = {
    "--glare-color": glareColor,
    "--glare-opacity": glareOpacity,
    "--glare-size": `${glareSize}%`,
    "--transition-duration": `${transitionDuration}ms`,
  } as React.CSSProperties;

  return (
    <div
      ref={ref}
      className={`glare-hover-container ${className} ${
        isHovering ? "hover" : ""
      }`}
      style={{
        ...style,
        ...glareStyle,
        width,
        height,
        background,
        borderRadius,
        borderColor,
      }}
      onMouseMove={handleMouseMove}
      onMouseEnter={handleMouseEnter}
      onMouseLeave={handleMouseLeave}
    >
      {children}
      <div className="glare"></div>
    </div>
  );
};

export default GlareHover;
