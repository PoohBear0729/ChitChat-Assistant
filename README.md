# ChitChat-Assistant
A chat robot with auxiliary functions based on RK3568(一个带智能语音控制的聊天机器人基于RK3568)

**Description:**

This is a demo of chat robot which can also control io device, but now the functions are not complete. It will be completed respectively. Beause of the short of money haha... When the sensors are purchased, the rest of the function will be added.

**details:**

Thereinfo, the qwen2-7b-int8 is to used as a chat-model, gpt-sovits model as the tts model. the gpt-sovits audio model can be download in Huggingface Visit https://huggingface.co/models.

**environments:** 

python QT6.7.2 C++

**2024.9.4**

Now the functions include chat, short memory, chat history. You can use text chat with robot, responses with audio and text. The input audio function will be come soon by using INMP441 sensor, meanwhile, corresponding linux driver will be pushed.

**python end usage:**
```
pip install -r requirements.txt
```
```
cd GPT-SOVITS
python api.py -s "your_sovits" -g "your gpt_sovits" -dr "reference audio" -dt "reference text" -dl "language"
```
your_sovits, your gpt_sovits, are the path gpt-sovits audio models,details Visit https://github.com/RVC-Boss/GPT-SoVITS, reference audio are the path of Reference audio while the reference text are the corresponding text of the audio.

expamples:
```
python api.py -s "E:\ChitChat-Assistant\GPT-SoVITS-v2-240807\SoVITS_weights_v2\Furina_e8_s304.pth" -g  "E:\ChitChat-Assistant\GPT-SoVITS-v2-240807\GPT_weights_v2\Furina-e15.ckpt" -dr "1.wav" -dt "哎，人气太高也是一种苦恼，谁让我这么受欢迎呢？" -dl "zh"
```
