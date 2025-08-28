ffmpeg -framerate 60 -i frame_%d.png -c:v libx264 -pix_fmt yuv420p output.mp4
