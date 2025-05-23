import sys
import os
import json
from vosk import Model, KaldiRecognizer
import wave


def transcribe_audio(audio_path):
    if not os.path.exists("models/vosk-model-small-en-us-0.15"):
        print("Модель Vosk не найдена!", file=sys.stderr)
        return ""

    model = Model("models/vosk-model-small-en-us-0.15")

    try:
        wf = wave.open(audio_path, "rb")
    except Exception as e:
        print(f"Ошибка открытия файла: {e}", file=sys.stderr)
        return ""

    if wf.getnchannels() != 1 or wf.getsampwidth() != 2 or wf.getcomptype() != "NONE":
        print("Аудиофайл должен быть WAV форматом, 16kHz, 16 бит, моно", file=sys.stderr)
        return ""

    rec = KaldiRecognizer(model, wf.getframerate())
    rec.SetWords(True)

    results = []
    while True:
        data = wf.readframes(4000)
        if len(data) == 0:
            break
        if rec.AcceptWaveform(data):
            results.append(json.loads(rec.Result())["text"])

    results.append(json.loads(rec.FinalResult())["text"])
    return " ".join(results)


if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Укажите путь к WAV аудиофайлу", file=sys.stderr)
        sys.exit(1)

    result = transcribe_audio(sys.argv[1])
    print(result)