import re
import os

VERSION_FILE = 'src/version.h'
PLUGIN_CONFIG_FILE = 'project/addons/yaml/plugin.cfg'

def get_current_versions():
    try:
        with open(VERSION_FILE, 'r') as f:
            version_h = f.read()
        with open(PLUGIN_CONFIG_FILE, 'r') as f:
            plugin_cfg = f.read()

        code_version = re.search(r'GODOT_YAML_VERSION\s+"(.+)"', version_h)
        plugin_version = re.search(r'version="(.+)"', plugin_cfg)

        if not code_version or not plugin_version:
            raise ValueError("Unable to find version strings in files")

        return code_version.group(1), plugin_version.group(1)
    except FileNotFoundError as e:
        print(f"Error: File not found - {e.filename}")
        return None, None
    except Exception as e:
        print(f"Error reading versions: {e}")
        return None, None

def update_versions(new_version):
    try:
        # Update version.h
        with open(VERSION_FILE, 'r') as f:
            version_h = f.read()
        version_h = re.sub(r'(GODOT_YAML_VERSION\s+")(.+)(")',
                           lambda m: f'{m.group(1)}{new_version}{m.group(3)}',
                           version_h)
        with open(VERSION_FILE, 'w') as f:
            f.write(version_h)

        # Update plugin.cfg
        with open(PLUGIN_CONFIG_FILE, 'r') as f:
            plugin_cfg = f.read()
        plugin_cfg = re.sub(r'(version=")(.+)(")',
                            lambda m: f'{m.group(1)}{new_version}{m.group(3)}',
                            plugin_cfg)
        with open(PLUGIN_CONFIG_FILE, 'w') as f:
            f.write(plugin_cfg)

        print(f"Version updated to {new_version} in both files.")
        return True
    except Exception as e:
        print(f"Error updating versions: {e}")
        return False

def main():
    print("Current versions:")
    code_version, plugin_version = get_current_versions()
    if code_version is None or plugin_version is None:
        print("Unable to proceed due to errors.")
        return

    print(f"Code version: {code_version}")
    print(f"Plugin version: {plugin_version}")

    new_version = input('Enter new version number: ')
    confirm = input(f"Are you sure you want to update to version {new_version}? [y/n]: ")

    if confirm.lower() == 'y':
        if update_versions(new_version):
            print("Update completed successfully.")
        else:
            print("Update failed. No changes were made to the files.")
    else:
        print('Version update cancelled.')

if __name__ == "__main__":
    main()
