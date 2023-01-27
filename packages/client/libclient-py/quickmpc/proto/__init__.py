import sys
from pathlib import Path

# 自動生成されたgrpcコードを相対パスに手動で修正する必要があるが，
# ここでパスを追加することで手動変更を不要にする
sys.path.append(str(Path(__file__).parent))
