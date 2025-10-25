#include <iostream>
#include <filesystem>
#include <fstream>
#include <cstdlib>
#include <nlohmann/json.hpp>

namespace fs = std::filesystem;
using namespace std;
using json = nlohmann::json;

int main(int argc, char* argv[]) {
    if (argc < 3) {
        cerr << "Usage: processor <input_fule> <output_dir>\n";
        return 1;
    }

    std::string input_file = argv[1];
    std::string output_dir = argv[2];

    json status;

    try {
        fs::create_directories(output_dir);

        std::string base = fs::path(input_file).stem();

        status["status"] = "processing";
        status["input"] = input_file;
        status["output_dir"] = output_dir;
        std::ofstream(output_dir + "/status.json") << status.dump(4);

        // Thumbnail
        std::string thumb_cmd = "ffmpeg -y -i " + input_file +
                                " -ss 00:00:01 -vframes 1 " + 
                                output_dir + "/" + base + "_thumb.jpg";

        int thumb_res = system(thumb_cmd.c_str());
        if (thumb_res != 0) throw std::runtime_error("Failed to create thumbnail");

        // Transcode to MP4
        std::string mp4_cmd = "ffmpeg -y -i " + input_file + 
                              " -c:v libx264 -preset fast -crf 23 -c:a aac " +
                              output_dir + "/" + base + "_converted.mp4";

        int mp4_res = system(mp4_cmd.c_str());
        if (mp4_res != 0) throw std::runtime_error("Failed to create MP4");

        // Transcode to Webm
        std::string webm_cmd = "ffmpeg -y -i " + input_file +
                               " -c:v libvpx-vp9 -b:v 0 -crf 30 -c:a libopus " +
                               output_dir + "/" + base + "_converted.webm";

        int webm_res = system(webm_cmd.c_str());
        if (webm_res != 0) throw std::runtime_error("Failed to create WebM");

        // === Write final status ===
        status["status"] = "finished";
        status["files"] = {
            base + "_thumb.jpg",
            base + ".mp4",
            base + ".webm"
        };

        std::ofstream(output_dir + "/status.json") << status.dump(4);
        std::cout << "✅ Processing complete for " << input_file << std::endl;
                
    } catch (const std::exception& e) {
        status["status"] = "failed";
        status["error"] = e.what();
        std::ofstream(output_dir + "/status.json") << status.dump(4);
        std::cerr << "❌ Error: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        status["status"] = "failed";
        status["error"] = "Unknown error occurred";
        std::ofstream(output_dir + "/status.json") << status.dump(4);
        std::cerr << "❌ Unknown error occurred" << std::endl;
        return 1;
    }

    return 0;
}