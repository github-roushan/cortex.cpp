import { Config } from '@/domain/config/config.interface';
import { Injectable } from '@nestjs/common';
import os from 'os';
import { join } from 'node:path';
import { existsSync, promises } from 'node:fs';
import yaml from 'js-yaml';

@Injectable()
export class FileManagerService {
  private configFile = '.cortexrc';
  private cortexDirectoryName = 'cortex';
  private modelFolderName = 'models';
  private cortexCppFolderName = 'cortex-cpp';

  async getConfig(): Promise<Config> {
    const homeDir = os.homedir();
    const configPath = join(homeDir, this.configFile);

    if (!existsSync(configPath)) {
      const config = this.defaultConfig();
      await this.createFolderIfNotExist(config.dataFolderPath);
      await this.writeConfigFile(config);
      return config;
    }

    try {
      const content = await promises.readFile(configPath, 'utf8');
      const config = yaml.load(content) as Config;
      return config;
    } catch (error) {
      console.warn('Error reading config file. Using default config.');
      console.warn(error);
      const config = this.defaultConfig();
      await this.createFolderIfNotExist(config.dataFolderPath);
      await this.writeConfigFile(config);
      return config;
    }
  }

  private async writeConfigFile(config: Config): Promise<void> {
    const homeDir = os.homedir();
    const configPath = join(homeDir, this.configFile);

    // write config to file as yaml
    const configString = yaml.dump(config);
    await promises.writeFile(configPath, configString, 'utf8');
  }

  private async createFolderIfNotExist(dataFolderPath: string): Promise<void> {
    if (!existsSync(dataFolderPath)) {
      await promises.mkdir(dataFolderPath, { recursive: true });
    }

    const modelFolderPath = join(dataFolderPath, this.modelFolderName);
    const cortexCppFolderPath = join(dataFolderPath, this.cortexCppFolderName);
    if (!existsSync(modelFolderPath)) {
      await promises.mkdir(modelFolderPath, { recursive: true });
    }
    if (!existsSync(cortexCppFolderPath)) {
      await promises.mkdir(cortexCppFolderPath, { recursive: true });
    }
  }

  private defaultConfig(): Config {
    // default will store at home directory
    const homeDir = os.homedir();
    const dataFolderPath = join(homeDir, this.cortexDirectoryName);

    return {
      dataFolderPath,
    };
  }

  async getDataFolderPath(): Promise<string> {
    const config = await this.getConfig();
    return config.dataFolderPath;
  }
}