#include "AudioVariationGroup.h"
#include "../../utils/utils.h"
#include "mod/logger.h"

std::vector<std::string> GetSoundVariants(const std::string& fullPath)
{
    namespace fs = std::filesystem;
    std::vector<std::string> sounds;

    logger->Info("[Variants] Checking file: %s", fullPath.c_str());

    fs::path path(fullPath);

    if (!fs::exists(path))
    {
        logger->Error("[Variants] Base file NOT FOUND");
        return sounds; // não tem nem o primeiro, já retorna
    }

    // Remove extensão para poder gerar pull_over_vehicle2.wav, 3.wav...
    std::string baseName = path.replace_extension().string();
    std::string ext = ".wav"; // se quiser, posso detectar automaticamente dps

    logger->Info("[Variants] Base name: %s", baseName.c_str());
    logger->Info("[Variants] Extension: %s", ext.c_str());

    // 🔹 Carrega o arquivo original primeiro
    logger->Info("[Variants] ✔ Loaded original");
    sounds.push_back(fullPath);

    // 🔹 Começa procurar as variantes seguindo padrão *_2.wav, *_3.wav, ...
    for (int i = 2;; i++)
    {
        std::string candidate = baseName + std::to_string(i) + ext;

        logger->Info("[Variants] Checking: %s", candidate.c_str());

        if (!fs::exists(candidate))
        {
            logger->Error("[Variants] Not found. Stopping.");
            break;
        }

        logger->Info("[Variants] ✔ Found variant");
        sounds.push_back(candidate);
    }

    logger->Info("[Variants] Total loaded = %d", sounds.size());
    return sounds;
}

IAudio* AudioVariationGroup::GetRandomAudio()
{
    if (audios.empty()) return nullptr;

    // Gera um índice aleatório válido
    int index = getRandomNumber(0, static_cast<int>(audios.size()) - 1);

    auto audio = audios[index];

    return audio;
}

IAudio* AudioVariationGroup::PlayRandom()
{
    auto audio = GetRandomAudio();

    if (audio) audio->Play();

    return audio;
}

void AudioVariationGroup::LoadNewAudio(std::string src, bool in3d)
{
    auto audio = in3d ? menuSZK->GetOrLoad3DAudio(src) : menuSZK->GetOrLoadAudio(src);

    if (!audio)
    {
        logger->Error("[AudioVariationGroup] Error loading audio: %s", src.c_str());
        return;
    }

    audios.push_back(audio);
    logger->Info("[AudioVariationGroup] Registered audio: %s", src.c_str());
}

void AudioVariationGroup::FindAndLoadAudioVariants(std::string src, bool in3d)
{
    auto variants = GetSoundVariants(src);

    for (auto variant : variants) { LoadNewAudio(variant, in3d); }
}