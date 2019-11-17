import { NgModule } from '@angular/core';
import { Routes, RouterModule } from '@angular/router';
import { PacienteComponent } from './paciente/paciente.component';

const routes: Routes = [
  {
    path: '',
    data: {
      title: 'Cadastros'
    },
    children: [
      {
        path: 'pacientes',
        component: PacienteComponent,
        data: {
          title: 'Pacientes'
        }
      }
    ]
  }
];

@NgModule({
  imports: [RouterModule.forChild(routes)],
  exports: [RouterModule]
})
export class CadastrosRoutingModule {}
