# draw.R

data <- read.csv("result.csv", header = TRUE) # , row.names = 1) # rnormを読み込む

# pdf("result.pdf") # pdfファイルとしてグラフを出力する準備
png("result.png") # png

# par(mfrow = c(2,1))
plot(data$freq, data$gain, type = "l", log = "x") # Display gain vs freq
# plot(data$freq, data$phase, type = "l", log = "x") # Display phase vs freq

dev.off() # Close the device
