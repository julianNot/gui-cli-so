#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#include <string>
#include <vector>

struct Frame {
    std::string content;
    int frame_number;
    bool is_free;
    int page_number;
    int process_id;
    int segment_id;
};

class MemoryCalculator {
public:
    MemoryCalculator(const std::vector<Frame>& frames);
    int calculateAvailableMemory();
    int calculateMemoryUsedByProcess(int process_id);

private:
    std::vector<Frame> frames;
    static const int FRAME_SIZE = 4 * 1024;
};

// Funciones principales que se usarán en la CLI
bool memoryAllocation(int process_id);
void releaseMemory(int process_id);
bool memorySwap(int segmento, int pagina, int process_id);
int freeMem();

// Funciones auxiliares
std::vector<Frame> loadFramesFromJson(const std::string& filename);
std::vector<std::string> pagination(const std::string& text, int size);
int countLines(const std::string& filePath);
void uploadToRam(const std::vector<std::vector<std::string>>& segments, int process_id);
std::string getPage(int frame_number);
void updateTable(int segmento, int pagina, int process_id, int new_page_ram_frame);

#endif // MEMORY_MANAGER_H