using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Ragnar.Client
{
    class Storages : Dictionary<string, Unsafe.StorageInterfaceBase>
    {
        public static Storages Instance = new Storages();
    }
}
