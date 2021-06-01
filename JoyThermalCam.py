from PIL import Image, ImageOps
import numpy as np
from scipy import interpolate

# paste your array here and edit it to look like the following:
# imgArrayRaw = np.array([[23.75, 23.75, 22.75, 21.75, 21.50, 22.00, 22.00, 22.00], 
#                         [27.25, 26.75, 26.50, 22.75, 22.00, 22.25, 22.50, 22.25], 
#                         [26.25, 26.75, 26.75, 23.25, 24.25, 23.75, 22.50, 22.50], 
#                         [26.00, 27.00, 29.25, 29.25, 29.50, 29.00, 24.25, 23.00], 
#                         [26.25, 27.00, 28.25, 28.50, 29.50, 28.00, 24.25, 22.25], 
#                         [27.00, 26.75, 26.00, 23.25, 23.50, 23.75, 23.00, 22.50], 
#                         [27.25, 25.50, 23.50, 21.50, 22.00, 22.50, 22.00, 22.50], 
#                         [23.25, 21.75, 22.25, 22.00, 22.50, 21.50, 23.00, 22.50]])

imgArrayRaw = np.array([[23.75, 23.75, 22.75, 21.75, 21.50, 22.00, 22.00, 22.00], 
                        [27.25, 26.75, 26.50, 22.75, 22.00, 22.25, 22.50, 22.25], 
                        [26.25, 26.75, 26.75, 23.25, 24.25, 23.75, 22.50, 22.50], 
                        [26.00, 27.00, 29.25, 29.25, 29.50, 29.00, 24.25, 23.00], 
                        [26.25, 27.00, 28.25, 28.50, 29.50, 28.00, 24.25, 22.25], 
                        [27.00, 26.75, 26.00, 23.25, 23.50, 23.75, 23.00, 22.50], 
                        [27.25, 25.50, 23.50, 21.50, 22.00, 22.50, 22.00, 22.50], 
                        [23.25, 21.75, 22.25, 22.00, 22.50, 21.50, 23.00, 22.50]])

imgArrayRaw *= 100 # multiply by 100 to get rid of decimal place

# prominent Arduino map function :)
def _map(x, in_min, in_max, out_min, out_max):
    return ((x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min)

# map array onto desired values between 0 and 255
imgArray256 = _map(imgArrayRaw, imgArrayRaw.min(), imgArrayRaw.max(), 0, 255)
# round array to get rid of decimals
imgArray256 = imgArray256.round(0)

# rotate array
imgArray256 = np.rot90(imgArray256)

#img256 = imgArrayRaw
#print(imgArrayRaw - 1)

# load final image
finalImage = Image.fromarray(imgArray256)

# resize final image
newsize = (512, 512)
# alternative resizing without resampling
# finalImage = finalImage.resize(newsize, resample=Image.NEAREST)
finalImage = finalImage.resize(newsize)

# colorise final image
finalImage = finalImage.convert("L")
finalImage = ImageOps.colorize(finalImage, black ="midnightblue", white ="red")

# show and save final image
finalImage.show()
finalImage.save("image.png")
