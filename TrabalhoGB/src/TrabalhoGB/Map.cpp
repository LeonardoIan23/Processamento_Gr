#include "Map.h"
#include <fstream>   // Biblioteca para ler e escrever em arquivos
#include <sstream>   // Biblioteca para processar strings como se fossem fluxos de dados
#include <iostream>  // Biblioteca para mensagens de erro no console

/**
 * @brief Lê um arquivo de configuração de mapa e preenche uma struct MapData.
 * @param filename O nome do arquivo de mapa a ser lido (ex: "mapa.txt").
 * @param outMapData A struct MapData que será preenchida com os dados lidos.
 * @return true se o mapa for carregado com sucesso, false caso contrário.
 */
bool loadMap(const std::string& filename, MapData& outMapData) {
    // Tenta abrir o arquivo de mapa.
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "ERRO: Não foi possível abrir o arquivo de mapa: " << filename << std::endl;
        return false;
    }

    std::string line;
    
    // Lê o arquivo linha por linha
    while (std::getline(file, line)) {
        // Ignora linhas vazias ou comentários
        if (line.empty() || line[0] == '#') {
            continue;
        }

        std::stringstream ss(line);
        std::string keyword;
        ss >> keyword;

        if (keyword == "TILESET") {
            ss >> outMapData.tileset.filename 
               >> outMapData.tileset.num_tiles 
               >> outMapData.tileset.tile_width 
               >> outMapData.tileset.tile_height;
            // Agora que sabemos o número de tiles, podemos inicializar o vetor de tiles caminháveis.
            if (outMapData.tileset.num_tiles > 0) {
                outMapData.walkable_tiles.assign(outMapData.tileset.num_tiles, false);
            }
        }
        else if (keyword == "WALKABLE") {
            int tile_id;
            // Lê cada ID de tile e o marca como caminhável ('true').
            while (ss >> tile_id) {
                if (tile_id >= 0 && tile_id < outMapData.walkable_tiles.size()) {
                    outMapData.walkable_tiles[tile_id] = true;
                }
            }
        }
        else if (keyword == "ITEMS") {
            int num_items;
            ss >> num_items;
            outMapData.items.clear(); // Limpa a lista de itens para recarregar
            for (int i = 0; i < num_items; ++i) {
                // Pega a próxima linha não-comentada do arquivo
                while (std::getline(file, line) && (line.empty() || line[0] == '#')) {}
                
                std::stringstream item_ss(line);
                Item item;
                item_ss >> item.type >> item.x >> item.y;
                outMapData.items.push_back(item);
            }
        }
        else if (keyword == "MAP") {
            ss >> outMapData.width >> outMapData.height;
            outMapData.tile_grid.resize(outMapData.width * outMapData.height);
            for (int r = 0; r < outMapData.height; ++r) {
                // Pega a próxima linha não-comentada do arquivo
                while (std::getline(file, line) && (line.empty() || line[0] == '#')) {}

                std::stringstream map_ss(line);
                for (int c = 0; c < outMapData.width; ++c) {
                    map_ss >> outMapData.tile_grid[r * outMapData.width + c];
                }
            }
        }
    }

    file.close();
    std::cout << "Arquivo de mapa '" << filename << "' lido com sucesso." << std::endl;
    return true;
}