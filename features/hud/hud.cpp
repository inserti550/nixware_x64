#include "hud.h"

static void hud::render_log()
{

    for (int i = 0; i < min((int)globals::kill_log.size(), settings::miscellaneous::globals::logs::lines); ++i)
    {
        std::string_view line = globals::kill_log[(int)globals::kill_log.size() - min((int)globals::kill_log.size(), settings::miscellaneous::globals::logs::lines) + i];
        float x = 1.f;
        float y = 1.f + i * ImGui::GetFontSize() + 2.f;

        ImVec4 color = { 1.f, 1.f, 1.f, 1.f };
        size_t pos = 0;

        while (pos < line.size())
        {
            size_t tag = line.find('{', pos);
            size_t text_end = (tag == std::string_view::npos) ? line.size() : tag;

            if (text_end > pos)
            {
                std::string_view segment = line.substr(pos, text_end - pos);
                ImU32 col32 = ImGui::ColorConvertFloat4ToU32(color);

                ImGui::GetBackgroundDrawList()->AddText(ImGui::GetFont(), ImGui::GetFontSize(), { x + 1.f, y + 1.f }, IM_COL32(0, 0, 0, 180),
                    segment.data(), segment.data() + segment.size());
                ImGui::GetBackgroundDrawList()->AddText(ImGui::GetFont(), ImGui::GetFontSize(), { x, y }, col32,
                    segment.data(), segment.data() + segment.size());

                x += ImGui::GetFont()->CalcTextSizeA(ImGui::GetFontSize(), FLT_MAX, 0.f,
                    segment.data(), segment.data() + segment.size()).x;

                pos = text_end;
            }

            if (tag == std::string_view::npos)
                break;

            constexpr std::string_view prefix = "{color:";
            if (line.substr(tag, prefix.size()) != "{color:") { pos = tag + 1; continue; }

            size_t close = line.find('}', tag + prefix.size());
            if (close == std::string_view::npos) { pos = tag + 1; continue; }

            ImVec4 c = { 1.f, 1.f, 1.f, 1.f };
            float* comps[] = { &c.x, &c.y, &c.z, &c.w };
            size_t cur = tag + prefix.size();
            bool ok = true;

            for (int j = 0; j < 4 && cur < close; ++j)
            {
                while (cur < close && line[cur] == ' ') ++cur;
                auto [ptr, ec] = std::from_chars(line.data() + cur, line.data() + close, *comps[j]);
                if (ec != std::errc{}) { ok = false; break; }
                cur = ptr - line.data();
                while (cur < close && (line[cur] == ',' || line[cur] == ' ')) ++cur;
                if (j == 2) break;
                if (cur >= close) break;
            }

            if (!ok) { pos = tag + 1; continue; }

            color = c;
            pos = close + 1;
        }
    }
}

void hud::render()
{
	if (settings::miscellaneous::globals::logs::enable)
        render_log();
}